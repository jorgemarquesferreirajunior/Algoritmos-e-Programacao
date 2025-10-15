#include "Globals.hpp"

/*
| Endereço | Tipo           | Função                                    |
| -------- | -------------- | ----------------------------------------- |
| 00001    | Coil           | LED1 ON/OFF                               |
| 00002    | Coil           | LED2 ON/OFF                               |
| 00003    | Coil           | LED3 ON/OFF                               |
| 00004    | Coil           | LED4 ON/OFF                               |
| 00005    | Coil           | Motor ON/OFF                              |
| 40001    | Holding Reg.   | Velocidade setpoint motor (0–100%)        |
| 40002    | Holding Reg.   | Contador peças tipo 1                     |
| 40003    | Holding Reg.   | Contador peças tipo 2                     |
| 10001    | Discrete Input | Sensor 1 (entrada esteira)                |
| 10002    | Discrete Input | Sensor 2 (peça tipo 1)                    |
| 10003    | Discrete Input | Sensor 3 (peça tipo 2)                    |
| 10004    | Discrete Input | Sensor 4 (saída esteira)                  |
| 30001    | Input Reg.     | Velocidade atual do motor (potenciômetro) |


| Tipo              | Pinos                          |
| :---------------- | :----------------------------- |
| Entradas digitais | 16, 17, 18, 19, 21, 22, 23, 25 |
| Entrada analógica | 34                             |
| Saídas digitais   | 26, 27, 32, 33                 |


*/

// Definições das variáveis
byte SLAVE_ADDRRES                      = 0x06;
const byte BROADCAST_ADDRESS            = 0x00;
const int BAUDRATE                      = 9600;

const unsigned short START_ADDR_COILS   = 0x10;
const unsigned short START_ADDR_DINPUTS = 0x20;
const unsigned short START_ADDR_HREGS   = 0x30;
const unsigned short START_ADDR_IREGS   = 0x40;

// Pinout do hardware - saidas digitais
extern const unsigned short STATUS_MOTOR    = 4;
extern const unsigned short STATUS_RECVDATA = 26;
extern const unsigned short STATUS_RESPDATA = 27;
extern const unsigned short STATUS_MACHINE  = 32;
extern const unsigned short RS485_ENABLE    = 33;
// Pinout do hardware - entradas digitais
extern const unsigned short INIT_PROCESS    = 13;
extern const unsigned short PART_AT_ENTRY   = 16;
extern const unsigned short PART_TYPE1      = 17;
extern const unsigned short PART_TYPE2      = 18;
extern const unsigned short PART_TYPE3      = 19;
extern const unsigned short PART_AT_EXIT    = 21;
extern const unsigned short BINARY_ADDR1    = 22;
extern const unsigned short BINARY_ADDR2    = 23;
extern const unsigned short BINARY_ADDR3    = 25;
// Pinout do hardware - entradas analogicas
extern const unsigned short IN_SPEED_MOTOR  = 34;



byte COILS_MAP[N_COILS]                 = {
                                            STATUS_MOTOR,
                                            STATUS_RECVDATA,
                                            STATUS_RESPDATA,
                                            STATUS_MACHINE,
                                            RS485_ENABLE
                                          }; 
byte DINPUTS_MAP[N_DINPUTS]             = {
                                            PART_AT_ENTRY,
                                            PART_TYPE1,
                                            PART_TYPE2,
                                            PART_TYPE3,
                                            PART_AT_EXIT,
                                            BINARY_ADDR1,
                                            BINARY_ADDR2,
                                            BINARY_ADDR3,
                                          };
unsigned short HREGS_MAP[N_HREGS]       = {0, 0, 0};
unsigned short IREGS_MAP[N_IREGS]       = {IN_SPEED_MOTOR};

unsigned long endFrameTime              = 10;
unsigned long tempTime                  = 0;
unsigned long bytesAvailable            = 0;

byte receivedData[DATA_SIZE]            = {0};
byte respData[DATA_SIZE]                = {0};
unsigned short valueCrc                 = 0;

const byte ILLEGAL_FUNCTION             = 0x01;
const byte ILLEGAL_ADDRESS              = 0x02;
const byte ILLEGAL_VALUE                = 0x03;

int contador                            = 0;

Crc16 crc;



