#ifndef TITANIUM_PROTOCOL_GUARD
#define TITANIUM_PROTOCOL_GUARD

#include "esp_err.h"

#include "TitaniumPackage.h"


namespace ProtocolErrors {
    constexpr esp_err_t INVALID_START_BYTE      = -1; /**< Error code indicating invalid Start Byte. */
    constexpr esp_err_t INVALID_PAYLOAD_SIZE    = -2; /**< Error code indicating invalid payload size. */
    constexpr esp_err_t INVALID_COMMAND         = -3; /**< Error code indicating invalid command. */
    constexpr esp_err_t INVALID_MEMORY_AREA     = -4; /**< Error code indicating invalid memory area. */
    constexpr esp_err_t INVALID_PAYLOAD_POINTER = -5; /**< Error code indicating invalid payload pointer. */
    constexpr esp_err_t INVALID_CRC             = -6; /**< Error code indicating invalid CRC. */
    constexpr esp_err_t INVALID_END_BYTE        = -7; /**< Error code indicating invalid End Byte. */
}

/**
 * 
 */
class TitaniumProtocol{
    public:
    TitaniumProtocol(){};
    esp_err_t Decode(uint8_t *buffer, size_t size, std::unique_ptr<TitaniumPackage>& package);
    esp_err_t Encode(std::unique_ptr<TitaniumPackage>& package, uint8_t *buffer, uint16_t size);

    private:
    std::pair<int16_t, esp_err_t>   GetStarByteOffset(uint8_t *buffer, uint16_t buffer_size);
    std::pair<int16_t, esp_err_t>   GetPayloadLength(uint8_t* buffer);
    std::pair<command_e, esp_err_t> GetCommand(uint8_t* buffer);
    std::pair<uint8_t, esp_err_t>   GetMemoryArea(uint8_t* buffer);
    std::pair<uint8_t*, esp_err_t>  GetPayload(uint8_t* buffer);
    std::pair<uint32_t, esp_err_t>  GetCRC(uint8_t* buffer, uint16_t payload_size);
    std::pair<uint8_t, esp_err_t>   GetEndByte(uint8_t* buffer, uint16_t payload_size);
    esp_err_t ValidatePayloadLength(uint16_t data_length);
    esp_err_t ValidateCommand(command_e command);
    esp_err_t ValidateEndByte(uint8_t end_byte);
    esp_err_t ValidateMemoryArea(uint8_t memory_area);
    esp_err_t ValidatePayload(uint8_t* payload);
    esp_err_t ValidateCRC(uint32_t crc, uint8_t *buffer, uint16_t size);
    void EncodePayloadLength(uint8_t* buffer, uint16_t payload_length);
    void EncodeCRC(uint8_t* buffer, uint32_t crc);
};

#endif /* TITANIUM_PROTOCOL_GUARD */