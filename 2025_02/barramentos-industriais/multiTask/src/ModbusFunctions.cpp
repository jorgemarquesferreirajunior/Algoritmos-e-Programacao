#include "ModbusFunctions.hpp"
#include <Crc16.hpp>

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void awaitFrame() {
    bytesAvailable = 0;
    // Variáveis locais
    long milisegundos;   // Controle de tempo de fim de quadro Modbus RTU

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
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void sendException(byte* data, byte code) {
    respData[0] = data[0];
    respData[1] = data[1] | 0x80;
    respData[2] = code;
    valueCrc = crc.Modbus(respData, 0, 3);
    respData[3] = valueCrc & 0xFF;
    respData[4] = (valueCrc >> 8) & 0xFF;

    if (data[0] != BROADCAST_ADDRESS) {
        Serial.write(respData, 5);
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void blink(unsigned short pin, unsigned long period, unsigned short reps) {
    digitalWrite(pin, LOW);
    for (unsigned short i = 0; i < reps; i++) {
        digitalWrite(pin, HIGH);
        delay(period);
        digitalWrite(pin, LOW);
        delay(period);
    } 
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void readCoils(byte* data) {
    unsigned short start_addr = (data[2] << 8) + data[3];
    unsigned short qty_coils  = (data[4] << 8) + data[5];

    bool is_valid_value = (
        (qty_coils >= 0x01 && qty_coils <= 0x07D0) &&
        (qty_coils <= START_ADDR_COILS + N_COILS - start_addr));

    bool is_valid_address = (
        start_addr >= START_ADDR_COILS &&
        ((start_addr + qty_coils - 1) < (START_ADDR_COILS + N_COILS)));

    if (!is_valid_value) {
        sendException(data, ILLEGAL_VALUE);
        return;
    }

    if (!is_valid_address) {
        sendException(data, ILLEGAL_ADDRESS);
        return;
    }

    // quantidade de bytes necessario
    byte byte_count = (qty_coils + 7) / 8;
    
    respData[0] = data[0];
    respData[1] = data[1];
    respData[2] = byte_count;

    // Inicializacao dos bytes de status das coils
    for (int i = 0; i < byte_count; i++) {
        respData[3 + i] = 0x00;
    }

    // Leitura de cada coil
    for (unsigned short i = 0; i < qty_coils; i++){
        int coil_index = start_addr - START_ADDR_COILS + i;
        if (digitalRead(COILS_MAP[coil_index])) {
            respData[3 + (i / 8)] |= (1 << (i % 8));
        }
    }

    valueCrc = crc.Modbus(respData, 0, 3 + byte_count);
    respData[3 + byte_count] = valueCrc & 0xFF;
    respData[4 + byte_count] = (valueCrc >> 8) & 0xFF;

    // Resposta
    if (data[0] != BROADCAST_ADDRESS) {
        Serial.write(respData, 5 + byte_count);
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void readDiscreteInputs(byte* data) {
    unsigned short start_addr = (data[2] << 8) + data[3];
    unsigned short qty_dinputs  = (data[4] << 8) + data[5];

    bool is_valid_value = (
        (qty_dinputs >= 0x01 && qty_dinputs <= 0x7D00) &&
        (qty_dinputs <= START_ADDR_DINPUTS + N_DINPUTS - start_addr));

    bool is_valid_address = (
        start_addr >= START_ADDR_DINPUTS &&
        ((start_addr + qty_dinputs - 1) < (START_ADDR_DINPUTS + N_DINPUTS)));

    if (!is_valid_value) {
        sendException(data, ILLEGAL_VALUE);
        return;
    }

    if (!is_valid_address) {
        sendException(data, ILLEGAL_ADDRESS);
        return;
    }

    // quantidade de bytes necessario
    byte byte_count = (qty_dinputs + 7) / 8;
    
    respData[0] = data[0];
    respData[1] = data[1];
    respData[2] = byte_count;

    // Inicializacao dos bytes de status das discrete input
    for (int i = 0; i < byte_count; i++) {
        respData[3 + i] = 0x00;
    }

    // Leitura de cada discrete input
    for (unsigned short i = 0; i < qty_dinputs; i++){
        int dinput_index = start_addr - START_ADDR_DINPUTS + i;
        if (digitalRead(DINPUTS_MAP[dinput_index])) {
            respData[3 + (i / 8)] |= (1 << (i % 8));
        }
    }

    valueCrc = crc.Modbus(respData, 0, 3 + byte_count);
    respData[3 + byte_count] = valueCrc & 0xFF;
    respData[4 + byte_count] = (valueCrc >> 8) & 0xFF;

    // Resposta
    if (data[0] != BROADCAST_ADDRESS) {
        Serial.write(respData, 5 + byte_count);
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void readHoldingRegisters(byte* data) {
    unsigned short start_addr = (data[2] << 8) + data[3];
    unsigned short qty_regs  = (data[4] << 8) + data[5];

    bool is_valid_value = (
        (qty_regs >= 0x01 && qty_regs <= 0x007D) &&
        (qty_regs <= START_ADDR_HREGS + N_HREGS - start_addr));

    bool is_valid_address = (
        start_addr >= START_ADDR_HREGS &&
        ((start_addr + qty_regs - 1) < (START_ADDR_HREGS + N_HREGS)));

    if (!is_valid_value) {
        sendException(data, ILLEGAL_VALUE);
        return;
    }

    if (!is_valid_address) {
        sendException(data, ILLEGAL_ADDRESS);
        return;
    }

    // quantidade de bytes necessario
    byte byte_count = qty_regs * 2;
    
    respData[0] = data[0];
    respData[1] = data[1];
    respData[2] = byte_count;

    // Inicializacao dos bytes de status dos holding registers
    for (int i = 0; i < byte_count; i++) {
        respData[3 + i] = 0x00;
    }          

    // Leitura de cada holding register
    for (unsigned short i = 0; i < qty_regs; i++){
        unsigned short hreg_index = start_addr - START_ADDR_HREGS + i;
        respData[3 + i*2] = (HREGS_MAP[hreg_index] >> 8) & 0xFF;
        respData[4 + i*2] = HREGS_MAP[hreg_index] & 0xFF;
    }

    valueCrc = crc.Modbus(respData, 0, 3 + byte_count);
    respData[3 + byte_count] = valueCrc & 0xFF;
    respData[4 + byte_count] = (valueCrc >> 8) & 0xFF;

    // Resposta
    if (data[0] != BROADCAST_ADDRESS) {
        Serial.write(respData, 5 + byte_count);
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void readInputRegisters(byte* data) {
    unsigned short start_addr = (data[2] << 8) + data[3];
    unsigned short qty_regs  = (data[4] << 8) + data[5];

    bool is_valid_value = (
        (qty_regs >= 0x01 && qty_regs <= 0x007D) &&
        (qty_regs <= START_ADDR_IREGS + N_IREGS - start_addr));

    bool is_valid_address = (
        start_addr >= START_ADDR_IREGS &&
        ((start_addr + qty_regs - 1) < (START_ADDR_IREGS + N_IREGS)));

    if (!is_valid_value) {
        sendException(data, ILLEGAL_VALUE);
        return;
    }
    
    if (!is_valid_address) {
        sendException(data, ILLEGAL_ADDRESS);
        return;
    }
    
    // quantidade de bytes necessario
    byte byte_count = qty_regs * 2;
    
    respData[0] = data[0];
    respData[1] = data[1];
    respData[2] = byte_count;

    // Inicializacao dos bytes de status dos input registers
    for (int i = 0; i < byte_count; i++) {
        respData[3 + i] = 0x00;
    }          

    // Leitura de cada input register
    for (unsigned short i = 0; i < qty_regs; i++){
        unsigned short ireg_index = start_addr - START_ADDR_IREGS + i;
        unsigned short analog_value = analogRead(IREGS_MAP[ireg_index]);
        respData[3 + i*2] = (analog_value >> 8) & 0xFF;
        respData[4 + i*2] = analog_value & 0xFF;
    }

    valueCrc = crc.Modbus(respData, 0, 3 + byte_count);
    respData[3 + byte_count] = valueCrc & 0xFF;
    respData[4 + byte_count] = (valueCrc >> 8) & 0xFF;

    // Resposta
    if (data[0] != BROADCAST_ADDRESS) {
        Serial.write(respData, 5 + byte_count);
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void writeSingleCoil(byte* data) {
    unsigned short start_addr = (data[2] << 8) + data[3];
    unsigned short data_value = (data[4] << 8) + data[5];

    bool is_valid_value = (data_value == 0x0000 || data_value == 0xFF00);
    bool is_valid_address = (
        start_addr >= START_ADDR_COILS &&
        start_addr < (START_ADDR_COILS + N_COILS));

    if (!is_valid_value) {
        sendException(data, ILLEGAL_VALUE);
        return;
    }
    
    if (!is_valid_address) {
        sendException(data, ILLEGAL_ADDRESS);
        return;
    }

    // atualizacao da coil
    digitalWrite(
        COILS_MAP[start_addr - START_ADDR_COILS],
        data_value == 0xFF00 ? HIGH : LOW
    );
    
    // Resposta
    if (data[0] != BROADCAST_ADDRESS) {
        Serial.write(data, bytesAvailable);
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

