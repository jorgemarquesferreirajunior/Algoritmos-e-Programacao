#include "ServerModbusRTU.hpp"



ServerModbus::ServerModbus() {
    addres = 0x00;
    recv_crc = 0x00;
    send_crc = 0x00;
    qtd_functions = 0;
    for (unsigned short i = 0; i < MAX_VALID_FUNCTIONS; i++) valid_functions[i] = 0x00;
}

ServerModbus::ServerModbus(unsigned short slave_addres) : addres(slave_addres){
    recv_crc = 0x00;
    send_crc = 0x00;
    qtd_functions = 0;
    for (unsigned short i = 0; i < MAX_VALID_FUNCTIONS; i++) valid_functions[i] = 0x00;
}

ServerModbus::~ServerModbus() {
    delete[] discrete_inputs;
    discrete_inputs = nullptr;
    delete[] coils;
    coils = nullptr;
    delete[] input_regs;
    input_regs = nullptr;
    delete[] holding_regs;
    holding_regs = nullptr;
}

unsigned short ServerModbus::getServerAddress() const {
    return addres;
}

void ServerModbus::setServerAddress(unsigned short _addres) {
    addres = _addres;
}

void ServerModbus::addDiscreteInputs(DiscreteInput* _discrete_inputs) {
    discrete_inputs = _discrete_inputs;
}

void ServerModbus::addCoils(Coil* _coils) {
    coils = _coils;
}

void ServerModbus::addInputRegisters(InputRegister* _input_registers) {
    input_regs = _input_registers;
}

void ServerModbus::addHoldingRegisters(HoldingRegister* _holding_registers) {
    holding_regs = _holding_registers;
}

void ServerModbus::await(unsigned int endFrameTime) {
    // Variáveis locais
  long milisegundos;   // Controle de tempo de fim de quadro Modbus RTU

  // *****************************************************
  // BLOCO DE COMUNICAÇÃO
  // Aguarda o início da recepção de um quadro Modbus RTU
  if (Serial.available() > 0) {  // Se há bytes no buffer de recepção
    bytes_availables = Serial.available();  // Armazena a quantidade inicial de bytes
    milisegundos = millis();              // Armazena o tempo atual

    // O quadro Modbus RTU termina quando:
    // 1. Não há mais recepção de bytes, e
    // 2. O tempo sem recepção excede 3,5 bytes
    bool endFrame = false;  // Flag para indicar fim de quadro

    while (!endFrame) {
      // Se novos bytes chegaram, atualiza o tempo
      if (Serial.available() != bytes_availables) {
        bytes_availables = Serial.available();
        milisegundos = millis();
      }
      else {
        // Se o tempo sem recepção exceder o limite, considera fim de quadro
        if ((millis() - milisegundos) > endFrameTime) {
          endFrame = true;
        }
      }
    }

    bytes_availables = Serial.available();
    execution_code = 0x00;
  }
}

void ServerModbus::receiveData() {
    
    for (unsigned short i = 0; i < MAX_BUFFER; i++) data[i] = 0x00;
    // Neste ponto, entende-se que um quadro Modbus RTU completo foi recebido
    Serial.readBytes(data, bytes_availables);  // Lê os bytes recebidos

    // Calcula o CRC16 dos dados recebidos (incluindo o CRC recebido)
    recv_crc = crc.Modbus(data, 0, bytes_availables);
    // unsigned short recv_addres = data[0];
    // unsigned short function = data[1];
    // unsigned short start_addres = (data[2] << 8) + data[3];
    // unsigned short qtd_coils;

    // if (function == 0x01) qtd_coils = (data[4] << 8) + data[5]; 
   

    // Temp
    // if (bytesAvailable > 0) {
    //     data[bytesAvailable] = (recv_crc >> 8) & 0xFF;
    //     data[bytesAvailable + 1] = recv_crc & 0xFF;
    //     Serial.write(data,bytesAvailable + 2);
    //     for (unsigned short i = 0; i < MAX_BUFFER; i++) data[i] = 0x00;
    // }
    


    //bytes_availables = 0;
}

unsigned short ServerModbus::getRecvCrc() const{
    return recv_crc;
}

unsigned short ServerModbus::getSendCrc() const{
    return send_crc;
}

unsigned short ServerModbus::getBytesAvailable() const {
    return bytes_availables;
}
byte* ServerModbus::getData() {
    return data;
}

void ServerModbus::sendMessage() {
    // data[bytes_availables] = (recv_crc >> 8) & 0xFF;
    // data[bytes_availables + 1] = recv_crc & 0xFF;
    // Serial.write(data,bytes_availables + 2);
    Serial.write(data,bytes_availables);
    bytes_availables = 0;
    recv_crc = 1;
    for (unsigned short i = 0; i < MAX_BUFFER; i++) data[i] = 0x00;
}

void ServerModbus::cleanData() {
    bytes_availables = 0;
    recv_crc = 1;
    for (unsigned short i = 0; i < MAX_BUFFER; i++) data[i] = 0x00;
}

void ServerModbus::addValidFunctions(byte* functions, unsigned short n_functions) {
    qtd_functions = n_functions;
    for (unsigned short i = 0; i < MAX_VALID_FUNCTIONS; i++) valid_functions[i] = 0x00;
    for (unsigned short i = 0; i < qtd_functions; i++) valid_functions[i] = functions[i];
}

void ServerModbus::readCoils() {

    // CORRIGIR - VERIFICACAO DO VALOR DE LEITURA
    unsigned short value = (data[5] << 8) | (data[4] & 0xFF);

    if (!(0x00 < value and 0x07D0 > value)) {
        sendExceptionCode(0x02);
    }
    else {
        sendMessage();
    }
}

void ServerModbus::readHoldingRegisters() {
    //
}

bool ServerModbus::isValidFunction() {
    for (unsigned short i = 0; i < qtd_functions; i++) {
        if (valid_functions[i] == data[1]) {
            return true;
        }
    }
    return false;
}

void ServerModbus::writeSingleCoil() {
    /******************************
     * PDU - PROCOTOL DATA UNIT
     * BYTE[0]: SLAVE ADDRESS
     * BYTE[1]: FUNCTION CODE
     * BYTE[2-3]: OUTPUT ADDRESS
     * BYTE[4-5]: OUTPUT VALUE
    ******************************/
   unsigned short output_address ;// = (data[2] << 8) | data[3];
   output_address = data[2];
   output_address <<= 8;
   output_address += data[3]&0xFF;

    if (output_address > 0x0000 && output_address < coils->getSize())
    {
        unsigned short output_value;
        output_value = data[4];
        output_value <<= 8;
        output_value += data[5]&0xFF;
        
        if ((output_value == 0x00FF) || (output_value == 0xFF00))
        {
            unsigned int n_c = coils->getSize();
            byte temp[n_c] = {0};
            unsigned short* temp2 = {0};
            temp[0] = output_address - 1;
            temp[1] = coils->getHardwareAddr()[output_address - 1];
            (output_value == 0xFF00) ? temp[2] = 0x01 : temp[2] = 0x00;

            // sendMessage();
            digitalWrite(coils->getHardwareAddr()[output_address], (output_value == 0xFF00) ? HIGH : LOW);
            // sendMessage();
            temp2 = coils->getHardwareAddr();

            for (unsigned int i = 0; i < n_c; i++) temp[i] = temp2[i];

            Serial.write(temp, n_c);
        }
        else {
            sendExceptionCode(ILLEGAL_DATA_VALUE);
        }
        
    }
    else {
        sendExceptionCode(ILLEGAL_DATA_ADDRESS);
    }
    
}

void ServerModbus::handle() {
    switch (getData()[1])
        {
        case 0x01:
          readCoils();
          break;
        case 0x02:
          /* code */
          break;

        case 0x03:
          readHoldingRegisters();
          break;
        
        case 0x04:
          /* code */
          break;

        case 0x05:
          writeSingleCoil();
          break;
        
        default:
          break;
        }
}

void ServerModbus::sendExceptionCode(byte code) {
    byte data_exception[5] = {0};
    for (int i = 0; i < 5; i++) data_exception[i] = 0x00;

    //if (!isValidFunction() and data[0] != 0x00) {
    if (data[0] != 0x00) {
        
        data_exception[0] = data[0];
        data_exception[1] = data[1] | 0x80;
        data_exception[2] = code;
        send_crc = crc.Modbus(data_exception, 0, 3);
        data_exception[3] = send_crc & 0xFF;
        data_exception[4] = (send_crc >> 8) & 0xFF;

        Serial.write(data_exception, 5);
        
    }
}








// Modbus Data
// Constructors
Coil::Coil() {
    this->size = 8;
    this->startAddress = 0x01;
    data = new bool[this->size];
    for (unsigned short i = 0; i < this->size; i++) data[i] = false;
}

Coil::Coil(unsigned short start_address, unsigned short size) : startAddress(start_address){
    if (size > MAX_SIZE) {
        this->size = MAX_SIZE;
    }
    else {
        this->size = size;
    }
    data = new bool[this->size];
    for (unsigned short i = 0; i < this->size; i++) data[i] = false;
}

// Destructor
Coil::~Coil() {
    delete[] data;
    data = nullptr;

    delete[] hardware;
    hardware = nullptr;
}

// Methods
bool Coil::read(unsigned short address) {
    if (!contains(address)) {
        exception_code = ModbusExceptionCode::IllegalDataAddress;
        return false;
    }
    else {
        return data[address - startAddress];
        exception_code = ModbusExceptionCode::OK;
    }
}

void Coil::write(unsigned short address, bool value) {
    if (!contains(address)) {
        exception_code = ModbusExceptionCode::IllegalDataAddress;
    }
    else {
        exception_code = ModbusExceptionCode::OK;
        data[address - startAddress] = value;
    }
}

unsigned Coil::getStartAddress() const {
    return startAddress;
}

unsigned Coil::getSize() const {
    return size;
}

unsigned short* Coil::getHardwareAddr() const {
    return hardware;
}

bool Coil::contains(unsigned short address) const {
    return address >= startAddress && address < (startAddress + size);
}

void Coil::setHardwareAddr(unsigned short* h_data) {
    for (unsigned short i = 0; i < size; i++) hardware[i] = 0x00;
    for (unsigned short i = 0; i < size; i++) hardware[i] = h_data[i];
    
}