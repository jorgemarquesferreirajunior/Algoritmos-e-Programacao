#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <Arduino.h>
#include <Crc16.hpp>
#include <cstddef>

// Variáveis Modbus
extern byte SLAVE_ADDRRES;
extern const byte BROADCAST_ADDRESS;
extern const int BAUDRATE;

// Endereços iniciais
extern const unsigned short START_ADDR_COILS;
extern const unsigned short START_ADDR_DINPUTS;
extern const unsigned short START_ADDR_HREGS;
extern const unsigned short START_ADDR_IREGS;

// Tamanhos dos arrays
constexpr size_t N_COILS        = 5;
constexpr size_t N_DINPUTS      = 9;
constexpr size_t N_HREGS        = 3;
constexpr size_t N_IREGS        = 1;

// Mapa dos registradores
extern byte COILS_MAP[N_COILS];
extern byte DINPUTS_MAP[N_DINPUTS];
extern unsigned short HREGS_MAP[N_HREGS];
extern unsigned short IREGS_MAP[N_IREGS];
// Pinout do hardware - saidas digitais
extern const unsigned short STATUS_MOTOR;
extern const unsigned short STATUS_RECVDAT;
extern const unsigned short STATUS_RESPDATA;
extern const unsigned short STATUS_MACHINE;
extern const unsigned short RS485_ENABLE;
// Pinout do hardware - entradas digitais
extern const unsigned short INIT_PROCESS;
extern const unsigned short PART_AT_ENTRY;
extern const unsigned short PART_TYPE1;
extern const unsigned short PART_TYPE2;
extern const unsigned short PART_TYPE3;
extern const unsigned short PART_AT_EXIT;
extern const unsigned short BINARY_ADDR1;
extern const unsigned short BINARY_ADDR2;
extern const unsigned short BINARY_ADDR3;
// Pinout do hardware - entradas analogicas
extern const unsigned short IN_SPEED_MOTOR;



// Variáveis de controle do Modbus
extern unsigned long endFrameTime;
extern unsigned long tempTime;
extern unsigned long bytesAvailable;

constexpr size_t DATA_SIZE = 20;
extern byte receivedData[DATA_SIZE];
extern byte respData[DATA_SIZE];
extern unsigned short valueCrc;

// Códigos de exceção
extern const byte ILLEGAL_FUNCTION;
extern const byte ILLEGAL_ADDRESS;
extern const byte ILLEGAL_VALUE;

// Variável compartilhada das tasks
extern int contador;

// Objeto CRC
extern Crc16 crc;

#endif
