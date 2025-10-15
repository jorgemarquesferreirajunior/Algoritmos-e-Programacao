#include <Arduino.h>
#include <Crc16.hpp>
#include "Globals.hpp"
#include "MyTasks.hpp"
#include "ModbusFunctions.hpp"
#include "esp32-hal-gpio.h"

void setSlaveAddress(void);

// Laço principal
void taskModbus(void *pvParameters) {

    setSlaveAddress();

    while (true) {

    bytesAvailable = 0;

    // Aguarda a recepcao completa de um quadro
    awaitFrame();

    // Lê os bytes recebidos
    Serial.readBytes(receivedData, Serial.available());

    // Calcula o CRC16 dos dados recebidos (incluindo o CRC recebido)
    valueCrc = crc.Modbus(receivedData, 0, bytesAvailable);

    // Se o CRC estiver correto (valor calculado == 0), o quadro é válido
    if (valueCrc == 0) {
        // Pisca o LED de status
        // blink(LED2_STATUS, 250, 1);
        // Verifica se o endereço é correspondente ao do escravo ou é um broadcast
        if (receivedData[0] == SLAVE_ADDRRES || receivedData[0] == 0) {
            // Verifica a funcao correspondente recebida no quadro 
            switch (receivedData[1])
            {
            case 0x01: // Funcao valida: Leitura de coils
                readCoils(receivedData);
                break;

            case 0x02: // Funcao valida: Leitura de discrete inputs
                readDiscreteInputs(receivedData);
                break;

            case 0x03: // Funcao valida: Leitura de holding registers
                readHoldingRegisters(receivedData);
                break;
            
            case 0x04: // Funcao valida: Leitura de input registers
                readInputRegisters(receivedData);
                break;

            case 0x05: // Funcao valida: Escrita de uma coil
                writeSingleCoil(receivedData);
                break;

            default: // erro de excecao (funcao invalida): 0x01
                sendException(receivedData, ILLEGAL_FUNCTION);
            }
        }
    }

    memset(receivedData, 0x00, sizeof(receivedData));
    memset(respData, 0x00, sizeof(respData));
    vTaskDelay(1);
    }
}

void setSlaveAddress(void)
{
    SLAVE_ADDRRES = (digitalRead(DINPUTS_MAP[5]) << 0) |
                    (digitalRead(DINPUTS_MAP[6]) << 1) |
                    (digitalRead(DINPUTS_MAP[7]) << 2);
}







