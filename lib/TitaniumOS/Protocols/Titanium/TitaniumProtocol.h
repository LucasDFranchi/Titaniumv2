#ifndef TITANIUM_PROTOCOL_H
#define TITANIUM_PROTOCOL_H

#include "TitaniumPackage.h"
#include "Kernel/error/error_enum.h"

namespace ProtocolConstants {
    constexpr uint8_t ACK[]              = {0x02, 0x00, 0x03, 0x41, 0x00, 0x41, 0x43, 0x4B, 0xB4, 0x43, 0xBA, 0x3B, 0x03};  // "ACK"
    constexpr uint8_t NAK[]              = {0x02, 0x00, 0x03, 0x41, 0x00, 0x4E, 0x41, 0x4B, 0x8D, 0x29, 0x9F, 0x84, 0x03};  // "NAK"
    constexpr uint16_t BROADCAST_ADDRESS = 0;                                                                               // "Address used for broadcast operations"
}  // namespace ProtocolConstants

namespace ProtocolErrors {
    constexpr titan_err_t INVALID_START_BYTE      = -1; /**< Error code indicating invalid Start Byte. */
    constexpr titan_err_t INVALID_PAYLOAD_SIZE    = -2; /**< Error code indicating invalid payload size. */
    constexpr titan_err_t INVALID_COMMAND         = -3; /**< Error code indicating invalid command. */
    constexpr titan_err_t INVALID_MEMORY_AREA     = -4; /**< Error code indicating invalid memory area. */
    constexpr titan_err_t INVALID_ADDRESS         = -5; /**< Error code indicating invalid address. */
    constexpr titan_err_t INVALID_PAYLOAD_POINTER = -6; /**< Error code indicating invalid payload pointer. */
    constexpr titan_err_t INVALID_CRC             = -7; /**< Error code indicating invalid CRC. */
    constexpr titan_err_t INVALID_END_BYTE        = -8; /**< Error code indicating invalid End Byte. */
    constexpr titan_err_t INVALID_UUID            = -9; /**< Error code indicating invalid UUID. */
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
    TitaniumProtocol() {};

   public:
    titan_err_t Decode(uint8_t* buffer, size_t size, std::unique_ptr<TitaniumPackage>& package);
    uint16_t Encode(std::unique_ptr<TitaniumPackage>& package, uint8_t* buffer, uint16_t size);

   private:
    uint16_t GetStarByteOffset(uint8_t* buffer, uint16_t buffer_size);
    uint32_t GetUUID(uint8_t* buffer, uint16_t remaining_bytes);
    uint16_t GetPayloadLength(uint8_t* buffer, uint16_t remaining_bytes);
    command_e GetCommand(uint8_t* buffer, uint16_t remaining_bytes);
    uint8_t GetMemoryArea(uint8_t* buffer, uint16_t remaining_bytes);
    uint16_t GetAddress(uint8_t* buffer, uint16_t remaining_bytes);
    uint8_t* GetPayload(uint8_t* buffer, uint16_t remaining_bytes);
    uint32_t GetCRC(uint8_t* buffer, uint16_t payload_size, uint16_t remaining_bytes);
    uint8_t GetEndByte(uint8_t* buffer, uint16_t payload_size, uint16_t remaining_bytes);
    titan_err_t ValidateUUID(uint32_t uuid);
    titan_err_t ValidatePayloadLength(uint16_t data_length);
    titan_err_t ValidateCommand(command_e command);
    titan_err_t ValidateEndByte(uint8_t end_byte);
    titan_err_t ValidateMemoryArea(uint8_t memory_area);
    titan_err_t ValidateAddress(uint16_t address);
    titan_err_t ValidatePayload(uint8_t* payload);
    titan_err_t ValidateCRC(uint32_t crc, uint8_t* buffer, uint16_t size);
    titan_err_t EncodeUUID(uint8_t* buffer, uint32_t uuid);
    titan_err_t EncodePayloadLength(uint8_t* buffer, uint16_t payload_length);
    titan_err_t EncodeAddress(uint8_t* buffer, uint16_t address);
    titan_err_t EncodeCRC(uint8_t* buffer, uint16_t offset, uint32_t crc);
};

#endif /* TITANIUM_PROTOCOL_H */