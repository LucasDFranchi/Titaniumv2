#include "esp_err.h"

#ifndef SERIAL_PROTOCOL_GUARD
#define SERIAL_PROTOCOL_GUARD

constexpr uint8_t READ_OPERATION = 'R';
constexpr uint8_t WRITE_OPERATION = 'W'; 

typedef struct __attribute__((packed)) ProtocolData_s{
    uint8_t start_byte;
    uint16_t data_length;
    uint8_t command;
    uint8_t memory_area;
    uint8_t *data_pointer;
    uint32_t crc32;
    uint8_t end_byte;
} ProtocolData_st;

/**
 * 
 */
class SerialProtocol{
    public:
    SerialProtocol(){};
    esp_err_t ProcessIncomingMessage(uint8_t *bytes, size_t size);
    uint16_t  GenerateResponseMessage(uint8_t *response_data, uint16_t size, uint8_t memory_area);
    ProtocolData_st GetMessageIncoming() const &;
    ProtocolData_st GetMessageSending() const &;
    private:
    esp_err_t ValidateStartByte_(uint8_t start_byte);
    esp_err_t ValidateDataLength_(uint16_t data_length) ;
    esp_err_t ValidateCommand_(uint8_t command);
    esp_err_t ValidateCRC_(const ProtocolData_st* incoming_message, uint8_t *bytes);
    esp_err_t ValidateEndByte_(uint8_t end_byte);
    esp_err_t ValidateIncomingMessage(uint8_t *bytes);
    private:
    ProtocolData_st incoming_message_;
    ProtocolData_st sending_message_;
    uint8_t *bytes;

};

#endif /* SERIAL_PROTOCOL_GUARD */