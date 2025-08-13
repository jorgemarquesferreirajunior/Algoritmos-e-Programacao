/* Exemplo de recepção Modbus com temporização de fim de quadro
   para o protocolo Modbus RTU
   
   Configurar o Termite com 9600 8-N-2

   Para ligar o LED1      0x06 0x05 0x00 0x02 0xff 0x00 0x2c 0x4d 
   Para desligar o LED1:  0x06 0x05 0x00 0x02 0x00 0x00 0x6d 0xbd

   Prof. Gustavo Kunzel
   IFRS - Campus Farroupilha
*/

// Bibliotecas
#include <Crc16.hpp>  // Biblioteca para funções de cálculo do CRC-16

// Variáveis Modbus
#define ENDERECO_ESCRAVO 0x06  // Endereço Modbus do escravo
#define BAUDRATE 9600          // Taxa de comunicação em bps (funciona bem até 9600 bps)
Crc16 crc;                     // Instancia do objeto com funções para cálculo do CRC-16
unsigned short valueCrc;       // Variável que armazena o valor calculado do CRC-16 (short = 16 bits)

// Buffers de recepção e resposta
byte receivedData[20];  // Armazena os bytes do quadro recebido
byte respData[20];      // Vetor para montagem das respostas às requisições

// Definições e constantes
const int LED2_STATUS = 6;  // LED2 usado para indicar funcionamento do protocolo
const int LED1 = 5;        // LED1 no pino 5 do shield, representa o coil
// const int RS485_ENABLE = 2;     // Pino de controle de fluxo RS-485, se utilizado

/* Cálculo do tempo de fim de quadro Modbus RTU em ms
   Para 8-N-1 temos 11 bits/byte:
   (3,5 bytes * 11 bits/byte * 1000 ms/s) / 9600 bits/s ≈ 4 ms
*/
unsigned int endFrameTime = ((3.5 * 11 * 1000) / BAUDRATE) + 1;  // Soma 1 ms para garantir margem

// Inicialização
void setup() {
  Serial.begin(BAUDRATE, SERIAL_8N2);  // Inicia a comunicação serial com configuração 8N2

  // Configuração dos pinos
  pinMode(LED2_STATUS, OUTPUT);
  pinMode(LED1, OUTPUT);

  // Pino DE/RE da RS-485 (caso esteja usando um transceiver)
  // pinMode(RS485_ENABLE, OUTPUT);
  // digitalWrite(RS485_ENABLE, LOW);  // Modo recepção
}

// Laço principal
void loop() {
  // Variáveis locais
  long milisegundos;   // Controle de tempo de fim de quadro Modbus RTU
  int bytesAvailable;  // Armazena a quantidade de bytes recebidos

  // *****************************************************
  // BLOCO DE COMUNICAÇÃO
  // Aguarda o início da recepção de um quadro Modbus RTU
  if (Serial.available() > 0) {  // Se há bytes no buffer de recepção

    bytesAvailable = Serial.available();  // Armazena a quantidade inicial de bytes
    milisegundos = millis();              // Armazena o tempo atual

    // O quadro Modbus RTU termina quando:
    // 1. Não há mais recepção de bytes, e
    // 2. O tempo sem recepção excede 3,5 bytes
    bool endFrame = false;  // Flag para indicar fim de quadro

    while (!endFrame) {
      // Se novos bytes chegaram, atualiza o tempo
      if (Serial.available() != bytesAvailable) {
        bytesAvailable = Serial.available();
        milisegundos = millis();
      } else {
        // Se o tempo sem recepção exceder o limite, considera fim de quadro
        if ((millis() - milisegundos) > endFrameTime) {
          endFrame = true;
        }
      }
    }

    // Neste ponto, entende-se que um quadro Modbus RTU completo foi recebido
      Serial.readBytes(receivedData, Serial.available());  // Lê os bytes recebidos

    // Teste de recepção: ecoa o quadro recebido
    // Serial.write(receivedData, bytesAvailable);

    // Calcula o CRC16 dos dados recebidos (incluindo o CRC recebido)
    valueCrc = crc.Modbus(receivedData, 0, bytesAvailable);

    // Se o CRC estiver correto (valor calculado == 0), o quadro é válido
    if (valueCrc == 0) {
      // Pisca o LED de status
      digitalWrite(LED2_STATUS, HIGH);
      delay(250);
      digitalWrite(LED2_STATUS, LOW);

      // Verifica se o endereço é correspondente ao do escravo ou é um broadcast
      if (receivedData[0] == ENDERECO_ESCRAVO || receivedData[0] == 0) {

        // Verifica se a função requisitada é "Write Single Coil" (0x05)
        if (receivedData[1] == 0x05) {
          // Extrai o endereço do registrador
          unsigned short endRegistrador = (receivedData[2] << 8) + receivedData[3];

          if (endRegistrador == 0x0002) {
            // Extrai o valor do coil
            unsigned short coilValue = (receivedData[4] << 8) + receivedData[5];

            // Se o valor é 0xFF00, liga o LED1
            if (coilValue == 0xFF00) {
              digitalWrite(LED1, HIGH);

              // Se não for broadcast, envia resposta de confirmação
              if (receivedData[0] != 0) {
                // A resposta do comando 0x05 é o próprio quadro de requisição
                // digitalWrite(RS485_ENABLE, HIGH);  // Modo transmissão RS-485
                Serial.write(receivedData, bytesAvailable);
                // Serial.flush();  // Aguarda envio completo
                // digitalWrite(RS485_ENABLE, LOW);   // Retorna para modo recepção
              }
            } else {
              if (coilValue == 0x0000) {
              digitalWrite(LED1, LOW);

              // Se não for broadcast, envia resposta de confirmação
              if (receivedData[0] != 0) {
                // A resposta do comando 0x05 é o próprio quadro de requisição
                // digitalWrite(RS485_ENABLE, HIGH);  // Modo transmissão RS-485
                Serial.write(receivedData, bytesAvailable);
                // Serial.flush();  // Aguarda envio completo
                // digitalWrite(RS485_ENABLE, LOW);   // Retorna para modo recepção
              }
            }
              /*
              else {
                // Valor inválido: envia quadro de exceção (código 0x03)
                respData[0] = receivedData[0];          // Endereço do escravo
                respData[1] = receivedData[1] | 0x80;   // Código da função com MSB = 1 (exceção)
                respData[2] = 0x03;                     // Código de exceção: valor inválido
                valueCrc = crc.Modbus(respData, 0, 3);  // Calcula CRC da resposta
                respData[3] = valueCrc & 0xFF;          // LSB do CRC
                respData[4] = (valueCrc >> 8) & 0xFF;   // MSB do CRC

                if (receivedData[0] != 0) {  // Se não for broadcast
                  // digitalWrite(RS485_ENABLE, HIGH);  // Modo transmissão
                  Serial.write(respData, 5);  // Envia resposta de exceção
                  // Serial.flush();                   // Aguarda envio completo
                  // digitalWrite(RS485_ENABLE, LOW);  // Retorna para modo recepção
                }
              }
              */
            }
          }

          }
        } else {
          // Função não suportada: envia quadro de exceção
          respData[0] = receivedData[0];          // Endereço do escravo
          respData[1] = receivedData[1] | 0x80;   // Código da função com MSB = 1 (exceção)
          respData[2] = 0x01;                     // Código de exceção: função inválida
          valueCrc = crc.Modbus(respData, 0, 3);  // Calcula CRC da resposta
          respData[3] = valueCrc & 0xFF;          // LSB do CRC
          respData[4] = (valueCrc >> 8) & 0xFF;   // MSB do CRC

          if (receivedData[0] != 0) {  // Se não for broadcast
            // digitalWrite(RS485_ENABLE, HIGH);  // Modo transmissão
            Serial.write(respData, 5);  // Envia resposta de exceção
            // Serial.flush();                   // Aguarda envio completo
            // digitalWrite(RS485_ENABLE, LOW);  // Retorna para modo recepção
          }
        }
      }
    }
  }

  // *****************************************************
  // BLOCO DE CONTROLE DE ENTRADAS E SAÍDAS
  // TODO: Implementar funcionalidades da aplicação, controle de I/Os
  // TODO: Tornar as variáveis relevantes em globais, para uso fora do bloco de comunicação
  // TODO: Lógicas de controle, intertravamento, setpoints, etc.
  // TODO: Seguir requisitos do projeto e fluxogramas do protocolo
  // TODO: Definir e validar valores dos registradores utilizados
  // TODO: Interpretar os comandos de acordo com a aplicação
  // TODO: Implementar resposta ou exceção conforme necessário
  // *****************************************************


//**********************************************************************
/*
  EXEMPLO DE TESTE DE CRC16

  // Declara um vetor de bytes com dados Modbus
  byte data[] = {0x01, 0x05, 0x01, 0x00, 0xff, 0x00};

  // Calcula o CRC16-Modbus dos dados
  value = crc.Modbus(data, 0, 6);  // (dados, índice inicial, tamanho)

  // Exibe o CRC16 calculado no monitor serial
  Serial.print("Modbus CRC = 0x");
  Serial.println(value, HEX);
*/



/*

TESTS
-> CRC
  -> Slave Address 
    -> Function: 0x01 function code not suported
      -> Output Address: 0x02 invalid output address
        -> Content: 0x03 output value not suported


*/





//**********************************************************************