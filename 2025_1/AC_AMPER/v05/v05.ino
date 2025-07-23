#include "LiquidCrystal_I2C.h"  // Biblioteca para controlar o display LCD via interface I2C

#define CALIBRACAO_LEITURA_VCC 1126400L  // Constante usada para calcular o Vcc com base na referência interna de 1.1V
#define NUMERO_AMOSTRAS 1480             // Número de amostras para calcular o valor eficaz (RMS)
#define PINO_LEITURA A0                  // Pino analógico onde o sensor de corrente está conectado
#define ADC_BITS 10                      // Resolução do conversor ADC do Arduino (10 bits)
#define ADC_COUNTS (1 << ADC_BITS)       // Valor máximo possível do ADC (2^10 = 1024)
#define TENSAO_REDE 220                  // Tensão da rede elétrica para calcular potência (220V)
#define MAX_CHAR 10                      // Tamanho máximo das strings de corrente/potência a exibir
// Constante de calibração do sensor de corrente:
// Considera a relação de transformação do transformador de corrente (2000:1) e o resistor shunt de 330 ohms.
// CONSTANTE_CALIBRACAO = 2000 / 330 ≈ 6.06
#define CONSTANTE_CALIBRACAO 6.06  // Constante que considera o numero de espiras enrolamento secundario do sensor e o resistor shunt do circuito
/*
 * Cálculo da corrente real (em Amperes) a partir da leitura do sensor:
 *
 * O sensor de corrente utilizado é do tipo transformador de corrente (CT),
 * onde a relação de espiras entre o primário e o secundário determina o valor da corrente secundária 
 * em função da corrente primária. No caso deste projeto:
 *
 *           I1      N2          100 A      N2
 *         -----  = ----  -->   ------ = ------  -->  N2 = 2000 espiras
 *           I2      N1          50 mA       1
 *
 * Ou seja, o secundário possui 2000 espiras em relação ao primário.
 * 
 * O sinal de saída do CT passa por um resistor shunt de 330 ohms, gerando uma tensão proporcional à corrente.
 * A relação entre a corrente primária e a tensão lida pelo ADC é então:
 *
 *          N2    V       2000    Amostra (Volts)
 *   I1 =  --- * ---- =   ---- * ------------------
 *          N1    R         1         330 ohms
 *
 * Considerando N2/N1 = 2000 e shunt de 330 ohms, temos:
 *
 *   I1 = (2000 / 330) * Amostra (em Volts)
 *
 * Portanto:
 *
 *   CONSTANTE_CALIBRACAO = 2000 / 330 ≈ 6.06
 *
 * Como o ADC mede em pontos digitais de 0 a 1023 (10 bits), é necessário converter a leitura para tensão real:
 *
 *   Tensao_real = (Vcc / ADC_COUNTS) * Amostra
 *
 * Multiplicando esta tensão pela constante de calibração (CONSTANTE_CALIBRACAO), obtém-se a corrente real:
 *
 *   Corrente_real = CONSTANTE_CALIBRACAO * Tensao_real
 *
 * onde CONSTANTE_CALIBRACAO já incorpora o fator de transformação do CT e o resistor shunt do circuito.
 */

LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Inicializa o LCD com endereço I2C 0x3F, 16 colunas e 2 linhas

// Variáveis principais
double TensaoOffset, CorrenteIrms, NovaCorrenteIrms, Potencia, NovaPotencia, epsilon = 0.0;
/*
 * TensaoOffset: Offset do sinal AC (normalmente Vcc/2)
 * CorrenteIrms: Valor anterior da corrente eficaz (RMS)
 * NovaCorrenteIrms: Valor atual da corrente eficaz
 * Potencia: Valor anterior da potência
 * NovaPotencia: Valor atual da potência
 * epsilon: Margem para evitar pequenas variações ou oscilações de leitura
 */

int TensaoFonte = 0;  // Tensão real de alimentação do Arduino (em mV)

// Buffers para armazenar as strings formatadas para exibição no LCD
char CorrenteIrms_str[MAX_CHAR], Potencia_str[MAX_CHAR];

long readVcc(void);  // Lê a tensão de alimentação do Arduino

void setup() {
  Serial.begin(9600);              // Inicia a comunicação serial a 9600 bps
  CorrenteIrms = 0.0;              // Inicializa corrente RMS com zero
  TensaoOffset = ADC_COUNTS >> 1;  // Inicializa o offset como Vcc/2 (equivalente a 512 para 10 bits)

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
  double TensaoFiltrada = 0;
  double SomatorioTensao = 0;

  // Realiza a aquisição de NUMERO_AMOSTRAS amostras para cálculo do valor eficaz
  for (unsigned int n = 0; n < NUMERO_AMOSTRAS; n++) {
    Amostra = analogRead(PINO_LEITURA);  // Lê uma amostra do sensor
    // Filtro IIR (Infinite Impulse Response) de primeira ordem: Ele é um tipo de filtro
    // digital que calcula cada nova saída como uma combinação da entrada atual e da saída anterior.
    // Atualiza lentamente o offset (componente DC) do sinal,
    // suavizando variações bruscas e ruído. Esse offset é subtraído da amostra
    // para isolar a componente AC do sinal do sensor de corrente.
    TensaoOffset = (TensaoOffset + (Amostra - TensaoOffset) / 1024);  // Filtro IIR simples para recalcular o offset
    TensaoFiltrada = Amostra - TensaoOffset;                          // Subtrai o offset para obter apenas o sinal AC
    SomatorioTensao += TensaoFiltrada * TensaoFiltrada;               // Soma dos quadrados (para cálculo do RMS)
  }

  TensaoFonte = readVcc();  // Lê a tensão de alimentação (em mV)
  // Calcula corrente RMS (A) com compensação do offset e fator de calibração
  NovaCorrenteIrms = CONSTANTE_CALIBRACAO * ((TensaoFonte / 1000.0) / (ADC_COUNTS)) * sqrt(SomatorioTensao / NUMERO_AMOSTRAS);

  if (NovaCorrenteIrms > 0.2) NovaCorrenteIrms -= 0.2;

  if (NovaCorrenteIrms <= 0.00) NovaCorrenteIrms = 0.0;  // Elimina ruídos com valores pequenos

  NovaPotencia = NovaCorrenteIrms * TENSAO_REDE;  // Calcula potência (P = I * V)

  if (abs(NovaCorrenteIrms - CorrenteIrms) > epsilon) CorrenteIrms = NovaCorrenteIrms;
  if (abs(NovaPotencia - Potencia) > epsilon) Potencia = NovaPotencia;

  CorrenteIrms *= 1000;  // Converte corrente para miliampères (mA)

  // Remove espaços iniciais (substitui por '0')
  dtostrf(CorrenteIrms, MAX_CHAR - 2, 2, CorrenteIrms_str);  // Converte double para string
  dtostrf(Potencia, MAX_CHAR - 2, 2, Potencia_str);

  for (int i = 0; i < MAX_CHAR - 2; i++) {
    if (CorrenteIrms_str[i] == ' ') CorrenteIrms_str[i] = '0';  // Substitui espaços por zeros
    if (Potencia_str[i] == ' ') Potencia_str[i] = '0';
  }

  // Exibe no LCD
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

  // Exibe na Serial
  Serial.println("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
  Serial.print("I:");
  Serial.print(CorrenteIrms);
  Serial.println("mA");
  Serial.print("P:");
  Serial.print(Potencia);
  Serial.println("Watss");
  Serial.println("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
  delay(500);  // Atualiza a cada 1/2 segundo
  TensaoOffset = ADC_COUNTS >> 1;
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
