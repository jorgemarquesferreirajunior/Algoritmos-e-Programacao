#include <Arduino.h>
#include <Crc16.hpp>

/* 
    Para ligar o LED1      0x06 0x05 0x00 0x02 0xff 0x00 0x2c 0x4d 
    Para desligar o LED1:  0x06 0x05 0x00 0x02 0x00 0x00 0x6d 0xbd
*/

// Definições e constantes Modbus
#define SLAVE_ADDRRES           0x06            // Endereço Modbus do escravo
#define BAUDRATE                9600            // Taxa de comunicação em bps (funciona bem até 9600 bps)
#define N_COILS                 0x06            // Quantidade de coils
#define START_ADDR_COILS        0x10            // Endereco inicial das coils
#define N_DINPUTS               0x06            // Quantidade de discrete inputs
#define START_ADDR_DINPUTS      0x20            // Endereco inicial das discrete inputs
#define N_HREGS                 0x03            // Quantidade de holding registers
#define START_ADDR_HREGS        0x30            // Endereco inicial dos holding registers
#define N_IREGS                 0x03            // Quantidade de input registers
#define START_ADDR_IREGS        0x40            // Endereco inicial dos input registers
#define BROADCAST_ADDRESS       0x00            // Endereco de broadcast
#define LED2_STATUS             0x0C            // LED2 usado para indicar funcionamento do protocolo
#define LED1                    0x05            // LED1 no pino 5 do shield, representa o coil
#define RS485_ENABLE            0x02            // Pino de controle de fluxo RS-485, se utilizado
#define ILLEGAL_FUNCTION        0x01            // Funcao invalida
#define ILLEGAL_ADDRESS         0x02            // Endereco invalido
#define ILLEGAL_VALUE           0x03            // Valor invalido

Crc16 crc;                                      // Instancia do objeto com funções para cálculo do CRC-16
unsigned short valueCrc;                        // Variável que armazena o valor calculado do CRC-16 (short = 16 bits)
unsigned int endFrameTime;                      // Armazena o tempo decorrido apos o recebimento de um byte
unsigned long tempTime;
byte receivedData[20];                          // Buffer de recepcao, armazena os bytes do quadro recebido
byte respData[20];                              // Buffer de resposta, vetor para montagem das respostas às requisições
int bytesAvailable;                             // Armazena a quantidade de bytes recebidos

// Mapeamento de hardware
const int COILS_MAP[N_COILS]    = {8,9,10,11,12,13};
const int DINPUTS_MAP[N_DINPUTS]= {2,3,4,5,6,7};
const int IREGS_MAP[N_IREGS]    = {A0, A1, A2};

// Mapeamento de softawre
unsigned short HREGS_MAP[N_HREGS]= {0};


/*
    ///////////////////////////////////////
    TESTES FUNCAO 0x01 : WRITE SINGLE COIL
    ///////////////////////////////////////

    Liga
    0x06 0x05 0x00 0x10 0xff 0x00 0x8c 0x48
    0x06 0x05 0x00 0x11 0xff 0x00 0xdd 0x88
    0x06 0x05 0x00 0x12 0xff 0x00 0x2d 0x88
    0x06 0x05 0x00 0x13 0xff 0x00 0x7c 0x48
    0x06 0x05 0x00 0x14 0xff 0x00 0xcd 0x89
    0x06 0x05 0x00 0x15 0xff 0x00 0x9c 0x49

    Desliga
    0x06 0x05 0x00 0x10 0x00 0x00 0xcd 0xb8
    0x06 0x05 0x00 0x11 0x00 0x00 0x9c 0x78
    0x06 0x05 0x00 0x12 0x00 0x00 0x6c 0x78
    0x06 0x05 0x00 0x13 0x00 0x00 0x3d 0xb8
    0x06 0x05 0x00 0x14 0x00 0x00 0x8c 0x79
    0x06 0x05 0x00 0x15 0x00 0x00 0xdd 0xb9

    Teste excecao 0x01: Funcao invalida
    0x06 0x0d 0x00 0x12 0xff 0x00 0xcc 0x49

    Teste excecao 0x02: Endereco invalido
    0x06 0x05 0x00 0x15 0xff 0x00 0x9c 0x49

    Teste excecao 0x03: Valor invalido
    0x06 0x05 0x00 0x12 0xff 0x02 0xac 0x49
*/

/*
    ///////////////////////////////////////
    TESTES FUNCAO 0x01 : READ COILS
    ///////////////////////////////////////

    0x06 0x01 0x00 0x10 0x00 0x04 0x3d 0xbb
    0x06 0x01 0x00 0x12 0x00 0x03 0xdd 0xb9
    0x06 0x01 0x00 0x10 0x00 0x05 0xfc 0x7b
    0x06 0x01 0x00 0x14 0x00 0x05 0xbd 0xba
    0x06 0x01 0x00 0x10 0x00 0x06 0xbc 0x7a

    Teste excecao 0x01: Funcao invalida
    0x06 0x0d 0x00 0x10 0x00 0x04 0x2d 0xba

    Teste excecao 0x02: Endereco invalido
    0x06 0x01 0x00 0x09 0x00 0x04 0xec 0x7c

    Teste excecao 0x03: Valor invalido
    0x06 0x01 0x00 0x10 0x00 0x0a 0xbc 0x7f

    /////////////////////////////////////////
    TESTES FUNCAO 0x02 : READ DISCRETE INPUTS
    /////////////////////////////////////////

    0x06 0x02 0x00 0x20 0x00 0x06 0xf8 0x75
    0x06 0x02 0x00 0x21 0x00 0x05 0xe9 0xb4
    0x06 0x02 0x00 0x25 0x00 0x06 0xe8 0x74

    Teste excecao 0x01: Funcao invalida
    0x06 0x0c 0x00 0x20 0x00 0x06 0x91 0xb4

    Teste excecao 0x02: Endereco invalido
    0x06 0x02 0x00 0x26 0x00 0x06 0x18 0x74

    Teste excecao 0x03: Valor invalido    0x06 0x02 0x00 0x20 0x00 0x07 0x39 0xb5

    /////////////////////////////////////////
    TESTES FUNCAO 0x02 : READ HOLDING REGISTERS
    /////////////////////////////////////////
    0x06 0x03 0x00 0x30 0x00 0x03 0x04 0x73

    /////////////////////////////////////////
    TESTES FUNCAO 0x02 : READ INPUT REGISTERS
    /////////////////////////////////////////
    0x06 0x04 0x00 0x40 0x00 0x01 0x31 0xa9
    0x06 0x04 0x00 0x40 0x00 0x03 0xb0 0x68
*/

// Prototipos
void awaitFrame();
void sendException(byte* data, byte code);
void blink(unsigned short pin, unsigned long period, unsigned short reps);
// Funcoes Modbus
void readCoils(byte* data);                     // Requisita a leitura de N* coils
void readDiscreteInputs(byte* data);            // Requisita a leitura de N* discrete inputs
void readHoldingRegisters(byte* data);          // Requisita a leitura de N* holding registers
void readInputRegisters(byte* data);            // Requisita a leitura de N* input registers
void writeSingleCoil(byte* data);               // Requisita a escrita de uma coil
void writeMultipleCoils(byte* data);            // Requisita a escrita de N* coils


// Inicialização
void setup() {
    Serial.begin(BAUDRATE, SERIAL_8N2);         // Inicia a comunicação serial com configuração 8N2

    // Configuração do hardware
    for (unsigned short i = 0; i < N_COILS; i++) pinMode(COILS_MAP[i], OUTPUT);
    for (unsigned short i = 0; i < N_DINPUTS; i++) pinMode(DINPUTS_MAP[i], INPUT_PULLUP);

    // Inicializacao dos vetores de recebimento e resposta
    memset(receivedData, 0x00, sizeof(receivedData));
    memset(respData, 0x00, sizeof(respData));
    memset(HREGS_MAP, 0x00, sizeof(HREGS_MAP));
    /*
    * Cálculo do tempo de fim de quadro Modbus RTU em ms
    * Para 8-N-1 temos 11 bits/byte:
    * (3,5 bytes * 11 bits/byte * 1000 ms/s) / 9600 bits/s ≈ 4 ms
    */
    endFrameTime = ((3.5 * 11 * 1000) / BAUDRATE);
    endFrameTime ++;                            // Soma 1ms para garantir o tempo minimo para termino do recebimento de dados
    
    HREGS_MAP[1] = 9500;

    tempTime = millis();

    // Pino DE/RE da RS-485 (caso esteja usando um transceiver)
    // pinMode(RS485_ENABLE, OUTPUT);
    // digitalWrite(RS485_ENABLE, LOW);  // Modo recepção
}

// Laço principal
void loop() {

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
            
            case 0x0F: // Funcao valida: Escrita de N* coils
                writeMultipleCoils(receivedData);
                break;

            default: // erro de excecao (funcao invalida): 0x01
                sendException(receivedData, ILLEGAL_FUNCTION);
            }
        }
    }

    memset(receivedData, 0x00, sizeof(receivedData));
    memset(respData, 0x00, sizeof(respData));
}





// Prototipos

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

void writeMultipleCoils(byte* data) {
    unsigned short start_addr = (data[2] << 8) + data[3];
    unsigned short qty_coils = (data[4] << 8) + data[5];
    unsigned short byte_count = data[6];
    unsigned short N = qty_coils / 8;

    if (qty_coils % 8 != 0) {
        N += 1;
    }

    bool is_valid_value = (
        qty_coils >= 0x01 &&
        qty_coils <= 0x07B0 &&
        N == byte_count);

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

    // atualizacao das coils
    for (unsigned short i = qty_coils - 1; i >= 0; i++) {

        unsigned short byte_index = i / 8;
        unsigned short bit_index = i % 8;

        bool coil_state = (data[7 + byte_index] >> bit_index) & 0x01;

        digitalWrite(
            COILS_MAP[start_addr - START_ADDR_COILS + i],
            coil_state ? HIGH : LOW
        );

        // Resposta
        respData[0] = data[0];                  // endereco do slave     
        respData[1] = data[1];                  // codigo da funcao     
        respData[2] = data[2];                  // endereco de inicio do registrador MSB
        respData[3] = data[3];                  // endereco de inicio do registrador LSB
        respData[4] = data[4];                  // quantidade de saidas MSB
        respData[5] = data[5];                  // quantidade de saidas LSB
        valueCrc = crc.Modbus(respData, 0, 6);
        respData[6] = valueCrc & 0xFF;          // CRC16 LSB
        respData[7] = (valueCrc >> 8) & 0xFF;   // CRC16 MSB
        
        if (data[0] != BROADCAST_ADDRESS) {
            Serial.write(respData, 8);
        }
    }    
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/