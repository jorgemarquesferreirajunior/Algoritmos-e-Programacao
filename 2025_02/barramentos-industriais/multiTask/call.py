#!/usr/bin/env python3
import serial
import time
import sys

def send_raw_bytes(byte_sequence, port, baudrate, bytesize, parity, stopbits, timeout):
    """
    Envia uma sequência de bytes raw para a porta serial
    
    Args:
        port: Nome da porta (ex: '/dev/ttyUSB0')
        baudrate: Velocidade (ex: 9600)
        byte_sequence: Lista de bytes ou string hexadecimal
    """
    try:
        # Converter string hexadecimal para lista de bytes se necessário
        if isinstance(byte_sequence, str):
            # Remover espaços e 0x prefix se houver
            hex_str = byte_sequence.replace(' ', '').replace('0x', '')
            bytes_to_send = bytes.fromhex(hex_str)
        else:
            bytes_to_send = bytes(byte_sequence)
        
        # print(f"bytes para micro: {bytes_to_send}")
        # Abrir porta serial
        with serial.Serial(port=port, baudrate=baudrate, bytesize=bytesize, parity=parity, stopbits=stopbits, timeout=timeout) as ser:
            # print(f"Conectado à porta {port}")
            
            # print(f"Enviando {len(bytes_to_send)} bytes: {bytes_to_send.hex(' ')}")
            
            # Enviar bytes
            ser.write(bytes_to_send)
            
            # Aguardar um pouco para resposta
            time.sleep(0.1)
            
            # Tentar ler resposta
            response = ser.read(100)  # Ler até 100 bytes
            if response:
                # print(f"Resposta recebida ({len(response)} bytes): {response.hex(' ')}")
                # print(f"Resposta ASCII: {response}")
                print(f"<: {response.hex(' ')}")
            else:
                print("Nenhuma resposta recebida")
                
    except serial.SerialException as e:
        print(f"Erro de serial: {e}")
    except Exception as e:
        print(f"Erro: {e}")

if __name__ == "__main__":
    # Configurações
    PORT = '/dev/ttyUSB0'  # Altere para sua porta
    BAUDRATE = 9600
    BYTESIZE=8
    PARITY='N'
    STOPBITS=1
    TIMEOUT=1

    while True:
        # Exemplo: enviar 0x01 0x02 0x01 0x00 0x01
        bytes_to_send = str(input("\n>: ")).strip().lower()
        # print("Bytes lidos: "+bytes_to_send)
        if bytes_to_send == "exit":
            break
        # Ou como string hexadecimal:
        # bytes_to_send = "01 02 01 00 01"
    
        send_raw_bytes(byte_sequence=bytes_to_send, port=PORT, baudrate=BAUDRATE, bytesize=BYTESIZE, parity=PARITY, stopbits=STOPBITS, timeout=TIMEOUT)
