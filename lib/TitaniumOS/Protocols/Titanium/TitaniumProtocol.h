#ifndef TITANIUM_PROTOCOL_H
#define TITANIUM_PROTOCOL_H

#include "esp_err.h"

#include "TitaniumPackage.h"

namespace ProtocolConstants {
    constexpr uint8_t ACK[] = {0x02, 0x00, 0x03, 0x41, 0x00, 0x41, 0x43, 0x4B, 0xB4, 0x43, 0xBA, 0x3B, 0x03};  // "ACK"
    constexpr uint8_t NAK[] = {0x02, 0x00, 0x03, 0x41, 0x00, 0x4E, 0x41, 0x4B, 0x8D, 0x29, 0x9F, 0x84, 0x03};  // "NAK"
}  // namespace ProtocolConstants

namespace ProtocolErrors {
    constexpr esp_err_t INVALID_START_BYTE      = -1; /**< Error code indicating invalid Start Byte. */
    constexpr esp_err_t INVALID_PAYLOAD_SIZE    = -2; /**< Error code indicating invalid payload size. */
    constexpr esp_err_t INVALID_COMMAND         = -3; /**< Error code indicating invalid command. */
    constexpr esp_err_t INVALID_MEMORY_AREA     = -4; /**< Error code indicating invalid memory area. */
    constexpr esp_err_t INVALID_ADDRESS         = -5; /**< Error code indicating invalid address. */
    constexpr esp_err_t INVALID_PAYLOAD_POINTER = -6; /**< Error code indicating invalid payload pointer. */
    constexpr esp_err_t INVALID_CRC             = -7; /**< Error code indicating invalid CRC. */
    constexpr esp_err_t INVALID_END_BYTE        = -8; /**< Error code indicating invalid End Byte. */
}  // namespace ProtocolErrors

/**
 * @class TitaniumProtocol
 * @brief Class for encoding and decoding Titanium protocol messages.
 */
class TitaniumProtocol {
   public:
    /**
     * @brief Default constructor for TitaniumProtocol.
     */
    TitaniumProtocol(){};

   public:
    esp_err_t Decode(uint8_t* buffer, size_t size, std::unique_ptr<TitaniumPackage>& package);
    uint16_t Encode(std::unique_ptr<TitaniumPackage>& package, uint8_t* buffer, uint16_t size);

   private:
    std::pair<int16_t, esp_err_t> GetStarByteOffset(uint8_t* buffer, uint16_t buffer_size);
    std::pair<int16_t, esp_err_t> GetPayloadLength(uint8_t* buffer);
    std::pair<command_e, esp_err_t> GetCommand(uint8_t* buffer);
    std::pair<uint8_t, esp_err_t> GetMemoryArea(uint8_t* buffer);
    std::pair<uint16_t, esp_err_t> GetAddress(uint8_t* buffer);
    std::pair<uint8_t*, esp_err_t> GetPayload(uint8_t* buffer);
    std::pair<uint32_t, esp_err_t> GetCRC(uint8_t* buffer, uint16_t payload_size);
    std::pair<uint8_t, esp_err_t> GetEndByte(uint8_t* buffer, uint16_t payload_size);
    esp_err_t ValidatePayloadLength(uint16_t data_length);
    esp_err_t ValidateCommand(command_e command);
    esp_err_t ValidateEndByte(uint8_t end_byte);
    esp_err_t ValidateMemoryArea(uint8_t memory_area);
    esp_err_t ValidateAddress(uint16_t address);
    esp_err_t ValidatePayload(uint8_t* payload);
    esp_err_t ValidateCRC(uint32_t crc, uint8_t* buffer, uint16_t size);
    void EncodePayloadLength(uint8_t* buffer, uint16_t payload_length);
    void EncodeAddress(uint8_t* buffer, uint16_t address);
    void EncodeCRC(uint8_t* buffer, uint32_t crc);
};

#endif /* TITANIUM_PROTOCOL_H */