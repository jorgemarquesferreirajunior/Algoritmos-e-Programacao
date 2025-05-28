#include "LiquidCrystal_I2C.h"  // Biblioteca para controlar o display LCD via interface I2C

#define CONSTANTE_CALIBRACAO 6.06        // Constante empírica para calibrar a leitura da corrente (baseada no sensor e circuito usado)
#define CALIBRACAO_LEITURA_VCC 1126400L  // Constante usada para calcular o Vcc com base na referência interna de 1.1V
#define NUMERO_AMOSTRAS 1480             // Número de amostras para calcular o valor eficaz (RMS)
#define PINO_LEITURA A0                  // Pino analógico onde o sensor de corrente está conectado
#define ADC_BITS 10                      // Resolução do conversor ADC do Arduino (10 bits)
#define ADC_COUNTS (1 << ADC_BITS)       // Valor máximo possível do ADC (2^10 = 1024)
#define TENSAO_REDE 220                  // Tensão da rede elétrica para calcular potência (220V)
#define MAX_CHAR 10                      // Tamanho máximo das strings de corrente/potência a exibir

LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Inicializa o LCD com endereço I2C 0x3F, 16 colunas e 2 linhas

double CorrenteOffset, CorrenteIrms, NovaCorrenteIrms, Potencia, NovaPotencia, epsilon = 0.0;
/*
CorrenteOffset: base da leitura do sensor (offset do sinal AC centrado em Vcc/2)
CorrenteIrms: valor anterior da corrente RMS
NovaCorrenteIrms: valor atual da corrente RMS
Potencia: valor anterior da potência
NovaPotencia: valor atual da potência
epsilon: tolerância para evitar oscilações desnecessárias
*/

int TensaoFonte = 0;  // Armazena o valor medido da tensão de alimentação (Vcc)

// Buffers para armazenar as strings formatadas para exibição no LCD
char CorrenteIrms_str[MAX_CHAR], Potencia_str[MAX_CHAR];

long readVcc(void);  // Lê a tensão de alimentação do Arduino

void setup() {
  Serial.begin(9600);                // Inicia a comunicação serial a 9600 bps
  CorrenteIrms = 0;                  // Inicializa corrente RMS com zero
  CorrenteOffset = ADC_COUNTS >> 1;  // Inicializa o offset como Vcc/2 (equivalente a 512 para 10 bits)

  lcd.init();       // Inicializa o LCD
  lcd.backlight();  // Liga a luz de fundo
  lcd.setCursor(0, 0);
  lcd.print(" INITIALIZING !");  // Mensagem temporária de inicialização
  lcd.setCursor(0, 1);
  delay(1500);  // Aguarda 1.5 segundos
  lcd.clear();  // Limpa o LCD
}

void loop() {
  double Amostra = 0;
  double CorrenteFiltrada = 0;
  double SomatorioCorrente = 0;

  for (unsigned int n = 0; n < NUMERO_AMOSTRAS; n++) {
    Amostra = analogRead(PINO_LEITURA);                                     // Lê uma amostra do sensor
    CorrenteOffset = (CorrenteOffset + (Amostra - CorrenteOffset) / 1024);  // Filtro IIR simples para recalcular o offset
    CorrenteFiltrada = Amostra - CorrenteOffset;                            // Subtrai o offset para obter apenas o sinal AC
    SomatorioCorrente += CorrenteFiltrada * CorrenteFiltrada;               // Soma dos quadrados para cálculo do RMS
  }

  TensaoFonte = readVcc();  // Lê a tensão de alimentação (em mV)
  NovaCorrenteIrms = CONSTANTE_CALIBRACAO * ((TensaoFonte / 1000.0) / (ADC_COUNTS)) * sqrt(SomatorioCorrente / NUMERO_AMOSTRAS);


  if (NovaCorrenteIrms <= 0.06) NovaCorrenteIrms = 0.0;  // Elimina ruídos com valores pequenos

  NovaPotencia = NovaCorrenteIrms * TENSAO_REDE;  // Calcula potência (P = I * V)

  if (abs(NovaCorrenteIrms - CorrenteIrms) > epsilon) CorrenteIrms = NovaCorrenteIrms;
  if (abs(NovaPotencia - Potencia) > epsilon) Potencia = NovaPotencia;

  if (CorrenteIrms <= 0.01) CorrenteIrms = 0.0;
  if (Potencia <= 0.06) Potencia = 0.0;

  CorrenteIrms *= 1000;  // Converte corrente para miliampères (mA)

  dtostrf(CorrenteIrms, MAX_CHAR - 2, 2, CorrenteIrms_str);  // Converte double para string
  dtostrf(Potencia, MAX_CHAR - 2, 2, Potencia_str);

  for (int i = 0; i < MAX_CHAR - 2; i++) {
    if (CorrenteIrms_str[i] == ' ') CorrenteIrms_str[i] = '0';  // Substitui espaços por zeros
    if (Potencia_str[i] == ' ') Potencia_str[i] = '0';
  }

  lcd.setCursor(0, 0);
  lcd.print("I:");
  lcd.print(CorrenteIrms_str);
  lcd.setCursor(11, 0);
  lcd.print("mA");

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(Potencia_str);
  lcd.setCursor(11, 1);
  lcd.print("Watts");

  delay(1000);  // Atualiza a cada 1 segundo
}

long readVcc() {
  long Resultado;

// Bloco para configurar o ADC para medir o canal da tensão de referência interna (1.1V)
// Seleciona a entrada interna de 1.1V para o ADC
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB &= ~_BV(MUX5);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
  MUX0

#endif

#if defined(__AVR__)
  delay(2);             // Aguarda estabilização
  ADCSRA |= _BV(ADSC);  // Inicia a conversão ADC
  while (bit_is_set(ADCSRA, ADSC))
    ;  // Espera até terminar

  Resultado = ADCL;        // Lê resultado do ADC (parte baixa)
  Resultado |= ADCH << 8;  // Lê parte alta e junta

  Resultado = CALIBRACAO_LEITURA_VCC / Resultado;  // Usa a constante para calcular o Vcc real
  return Resultado;                                // Retorna o Vcc em milivolts
#elif defined(__arm__)
  return (3300);  // Para placas ARM, retorna 3.3V fixo
#else
  return (3300);  // Fallback genérico
#endif
}
