#ifndef SERVER_MODBUS_RTU_HPP
#define SERVER_MODBUS_RTU_HPP
#include "Crc16.hpp"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#elif defined(ARDUINO)
  #include "WProgram.h"
#else
  #include <cstdint>
#endif

#define ILLEGAL_FUNCTION        0x01
#define ILLEGAL_DATA_ADDRESS    0x02
#define ILLEGAL_DATA_VALUE      0x03

enum class ModbusExceptionCode : byte {
    OK = 0x00,
    IllegalFunction = 0x01,
    IllegalDataAddress = 0x02,
    IllegalDataValue = 0x03
};


class DiscreteInput {
    //
};

class Coil {
    private:
        bool *data;
        unsigned short startAddress;
        unsigned short size;
        unsigned short* hardware;
        ModbusExceptionCode exception_code;
        static const unsigned short MAX_SIZE = 2000;

    public:
        // Constructor
        Coil();
        Coil(unsigned short start_address, unsigned short size);
        

        // Destructor
        ~Coil();

        // Methods
        bool read(unsigned short address);
        void write(unsigned short address, bool value);
        unsigned getStartAddress() const;
        unsigned short* getHardwareAddr() const;
        unsigned getSize() const;
        bool contains(unsigned short address) const;
        void setHardwareAddr(unsigned short* h_data);
};


class InputRegister {
    //
};

class HoldingRegister {
    //
};


class ServerModbus {
    private:
        unsigned short addres;
        unsigned short recv_crc = 1;
        unsigned short send_crc = 1;
        DiscreteInput* discrete_inputs;
        Coil* coils;
        InputRegister* input_regs;
        HoldingRegister* holding_regs;
        static const unsigned short MAX_BUFFER = 60;
        static const unsigned short MAX_VALID_FUNCTIONS = 21;
        byte data[MAX_BUFFER] = {0};
        byte valid_functions[MAX_VALID_FUNCTIONS] = {0};
        unsigned short qtd_functions;
        byte execution_code = 0x00;
        int bytes_availables = 0;
        Crc16 crc;
        
        
        
    public:
        // Constructors
        ServerModbus();
        ServerModbus(unsigned short slave_addres);
        ServerModbus(unsigned short slave_addres, DiscreteInput discrete_inputs);
        ServerModbus(unsigned short slave_addres, Coil coils);
        ServerModbus(unsigned short slave_addres, InputRegister input_registers);
        ServerModbus(unsigned short slave_addres, HoldingRegister holding_registers);
        
        // Destructors
        ~ServerModbus();

        // Methods
        unsigned short getServerAddress() const;
        unsigned short getRecvCrc() const;
        unsigned short getSendCrc() const;
        unsigned short getBytesAvailable() const;
        byte* getData();
        bool isValidFunction();
        void await(unsigned int endFrameTime);
        void receiveData();
        void setServerAddress(unsigned short _addres);
        void addDiscreteInputs(DiscreteInput* _discrete_inputs);
        void addCoils(Coil* _coils);
        void addInputRegisters(InputRegister* _input_registers);
        void addHoldingRegisters(HoldingRegister* _holding_registers);
        void addValidFunctions(byte* functions, unsigned short n_functions);
        void sendMessage();
        void cleanData();
        void readDiscreteInputs();
        void readCoils();
        void readHoldingRegisters();
        void writeSingleCoil();
        void sendExceptionCode(byte code);
        void handle();
        


};

#endif