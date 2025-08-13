#include <Arduino.h>
#include <Crc16.hpp>

/* 
    Para ligar o LED1      0x06 0x05 0x00 0x02 0xff 0x00 0x2c 0x4d 
    Para desligar o LED1:  0x06 0x05 0x00 0x02 0x00 0x00 0x6d 0xbd
*/

// Definições e constantes Modbus
#define SLAVE_ADDRRES           0x06            // Endereço Modbus do escravo
#define BAUDRATE                9600            // Taxa de comunicação em bps (funciona bem até 9600 bps)
#define N_COILS                 0x05            // Quantidade de coils
#define START_ADDR_COILS        0x10            // Endereco inicial das coils
#define BROADCAST_ADDRESS       0x00            // Endereco de broadcast
#define LED2_STATUS             0x0C            // LED2 usado para indicar funcionamento do protocolo
#define LED1                    0x05            // LED1 no pino 5 do shield, representa o coil
#define RS485_ENABLE            0x02            // Pino de controle de fluxo RS-485, se utilizado

Crc16 crc;                                      // Instancia do objeto com funções para cálculo do CRC-16
unsigned short valueCrc;                        // Variável que armazena o valor calculado do CRC-16 (short = 16 bits)
unsigned int endFrameTime;                      // Armazena o tempo decorrido apos o recebimento de um byte
unsigned long tempTime;
byte receivedData[20];                          // Buffer de recepcao, armazena os bytes do quadro recebido
byte respData[20];                              // Buffer de resposta, vetor para montagem das respostas às requisições
const int COILS_MAP[N_COILS] =                  // Mapeamento de hardware
                                {3,6,13,10,11};

/*
    TESTES FUNCAO 0x01 : WRITE SINGLE COIL

    Liga
    0x06 0x05 0x00 0x10 0xff 0x00 0x8c 0x48
    0x06 0x05 0x00 0x11 0xff 0x00 0xdd 0x88
    0x06 0x05 0x00 0x12 0xff 0x00 0x2d 0x88
    0x06 0x05 0x00 0x13 0xff 0x00 0x7c 0x48
    0x06 0x05 0x00 0x14 0xff 0x00 0xcd 0x89

    Desliga
    0x06 0x05 0x00 0x10 0x00 0x00 0xcd 0xb8
    0x06 0x05 0x00 0x11 0x00 0x00 0x9c 0x78
    0x06 0x05 0x00 0x12 0x00 0x00 0x6c 0x78
    0x06 0x05 0x00 0x13 0x00 0x00 0x3d 0xb8
    0x06 0x05 0x00 0x14 0x00 0x00 0x8c 0x79

    Teste excecao 0x01: Funcao invalida
    0x06 0x0d 0x00 0x12 0xff 0x00 0xcc 0x49

    Teste excecao 0x02: Endereco invalido
    0x06 0x05 0x00 0x15 0xff 0x00 0x9c 0x49

    Teste excecao 0x03: Valor invalido
    0x06 0x05 0x00 0x12 0xff 0x02 0xac 0x49
*/

/*
    TESTES FUNCAO 0x01 : READ COILS
    0x06 0x01 0x00 0x10 0x00 0x04 0x3d 0xbb
    0x06 0x01 0x00 0x12 0x00 0x03 0xdd 0xb9
    0x06 0x01 0x00 0x10 0x00 0x05 0xfc 0x7b

    Teste excecao 0x01: Funcao invalida
    0x06 0x0d 0x00 0x10 0x00 0x04 0x2d 0xba

    Teste excecao 0x02: Endereco invalido
    0x06 0x01 0x00 0x09 0x00 0x04 0xec 0x7c

    Teste excecao 0x03: Valor invalido
    0x06 0x01 0x00 0x10 0x00 0x0a 0xbc 0x7f
    

*/

// Prototipos
void awaitFrame(int* recv_bytes);
void writeSingleCoil(byte* data, int qtd_bytes);
void readCoils(byte* data);
void sendException(byte* data, byte code);
void blink(unsigned short pin, unsigned long period, unsigned short reps);

// Inicialização
void setup() {
    Serial.begin(BAUDRATE, SERIAL_8N2);         // Inicia a comunicação serial com configuração 8N2

    // Configuração do hardware
    pinMode(LED2_STATUS, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // Inicializacao dos vetores de recebimento e resposta
    memset(receivedData, 0x00, sizeof(receivedData));
    memset(respData, 0x00, sizeof(respData));

    /*
    * Cálculo do tempo de fim de quadro Modbus RTU em ms
    * Para 8-N-1 temos 11 bits/byte:
    * (3,5 bytes * 11 bits/byte * 1000 ms/s) / 9600 bits/s ≈ 4 ms
    */
    endFrameTime = ((3.5 * 11 * 1000) / BAUDRATE);
    endFrameTime ++;                            // Soma 1ms para garantir o tempo minimo para termino do recebimento de dados
    tempTime = millis();

    // Pino DE/RE da RS-485 (caso esteja usando um transceiver)
    // pinMode(RS485_ENABLE, OUTPUT);
    // digitalWrite(RS485_ENABLE, LOW);  // Modo recepção
}

// Laço principal
void loop() {

    int bytesAvailable = 0;

    // Aguarda a recepcao completa de um quadro
    awaitFrame(&bytesAvailable);

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

            case 0x05: // Funcao valida: Escrita de coils
                writeSingleCoil(receivedData, bytesAvailable);
                break;
            
            default: // erro de excecao (funcao invalida): 0x01
                sendException(receivedData, 0x01);
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

void awaitFrame(int* recv_bytes) {
    *recv_bytes = 0;
    // Variáveis locais
    long milisegundos;   // Controle de tempo de fim de quadro Modbus RTU

    // *****************************************************
    // BLOCO DE COMUNICAÇÃO
    // Aguarda o início da recepção de um quadro Modbus RTU
    if (Serial.available() > 0) {  // Se há bytes no buffer de recepção
        *recv_bytes = Serial.available();  // Armazena a quantidade inicial de bytes
        milisegundos = millis();              // Armazena o tempo atual

        // O quadro Modbus RTU termina quando:
        // 1. Não há mais recepção de bytes, e
        // 2. O tempo sem recepção excede 3,5 bytes
        bool endFrame = false;  // Flag para indicar fim de quadro

        while (!endFrame) {
            // Se novos bytes chegaram, atualiza o tempo
            if (Serial.available() != *recv_bytes) {
                *recv_bytes = Serial.available();
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

void writeSingleCoil(byte* data, int qtd_bytes) {
    unsigned short data_addr = (data[2] << 8) + data[3];
    unsigned short data_value = (data[4] << 8) + data[5];

    // validacao do endereco do registrador
    if (data_addr >= START_ADDR_COILS && data_addr < (START_ADDR_COILS + N_COILS)) {
        // validacao do valor do registrador
        if (data_value == 0x0000 || data_value == 0xFF00) {
            // atualizacao da coil
            digitalWrite(COILS_MAP[data_addr - START_ADDR_COILS], data_value == 0xFF00 ? HIGH : LOW);
            
            if (data[0] != BROADCAST_ADDRESS) {
                Serial.write(data, qtd_bytes);
            }
        }
        else {
            // erro de excecao (valor invalido): 0x03
            sendException(data, 0x03);
        }
    }
    else {
        // erro de excecao (endereco invalido): 0x02 
        sendException(data, 0x02);
    }
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

void readCoils(byte* data) {
    unsigned short start_addr = (data[2] << 8) + data[3];
    unsigned short qty_coils  = (data[4] << 8) + data[5];

    // validacao do endereco do registrador
    if (start_addr >= START_ADDR_COILS && start_addr < (START_ADDR_COILS + N_COILS)) {
        // validadcao da quantidade de coils a serem lidas
        if (qty_coils > 0x00 && qty_coils <= N_COILS) {
            // qunatidade de bytes necessario
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

            if (data[0] != BROADCAST_ADDRESS) {
                Serial.write(respData, 5 + byte_count);
            }

        }
        else {
            // erro de excecao (valor invalido): 0x03
            sendException(data, 0x03);
        }
    }
    else {
        // erro de excecao (endereco invalido): 0x02 
        sendException(data, 0x02);
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