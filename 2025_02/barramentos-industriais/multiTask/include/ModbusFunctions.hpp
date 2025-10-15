#ifndef MODBUS_FUNCTIONS_HPP
#define MODBUS_FUNCTIONS_HPP
#include <Arduino.h>
#include "Globals.hpp"

void awaitFrame();
void sendException(byte* data, byte code);
void blink(unsigned short pin, unsigned long period, unsigned short reps);

void readCoils(byte* data);                     // Requisita a leitura de N* coils
void readDiscreteInputs(byte* data);            // Requisita a leitura de N* discrete inputs
void readHoldingRegisters(byte* data);          // Requisita a leitura de N* holding registers
void readInputRegisters(byte* data);            // Requisita a leitura de N* input registers
void writeSingleCoil(byte* data);               // Requisita a escrita de uma coil

#endif // MODBUS_FUNCTIONS_HPP
