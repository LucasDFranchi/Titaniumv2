#include "TitaniumProtocol.h"
#include "Kernel/error/error_enum.h"
#include "Protocols/Titanium/Utils/CRCUtils.h"

namespace ProtocolAttributes {
    constexpr uint8_t START_BYTE_OFFSET     = 0;
    constexpr uint8_t START_BYTE_SIZE       = 1;
    constexpr uint8_t UUID_OFFSET           = START_BYTE_OFFSET + START_BYTE_SIZE;
    constexpr uint8_t UUID_SIZE             = 4;
    constexpr uint8_t PAYLOAD_LENGTH_OFFSET = UUID_OFFSET + UUID_SIZE;
    constexpr uint8_t PAYLOAD_LENGTH_SIZE   = 2;
    constexpr uint8_t COMMAND_OFFSET        = PAYLOAD_LENGTH_OFFSET + PAYLOAD_LENGTH_SIZE;
    constexpr uint8_t COMMAND_SIZE          = 1;
    constexpr uint8_t MEMORY_AREA_OFFSET    = COMMAND_OFFSET + COMMAND_SIZE;
    constexpr uint8_t MEMORY_AREA_SIZE      = 1;
    constexpr uint8_t ADDRESS_OFFSET        = MEMORY_AREA_OFFSET + MEMORY_AREA_SIZE;
    constexpr uint8_t ADDRESS_SIZE          = 2;
    constexpr uint8_t HEADER_OFFSET         = ADDRESS_OFFSET + ADDRESS_SIZE;
    constexpr uint8_t CRC_SIZE              = 4;
    constexpr uint8_t STATIC_MESSAGE_SIZE   = HEADER_OFFSET + CRC_SIZE;
    constexpr uint8_t END_BYTE_SIZE         = 1;

}  // namespace ProtocolAttributes

namespace Protocol {
    constexpr uint8_t START_BYTE            = 2;    /**< Start byte of the message. */
    constexpr uint8_t END_BYTE              = 3;    /**< End byte of the message. */
    constexpr uint16_t MAXIMUM_MESSAGE_SIZE = 1024; /**< Maximum size of a message. */

}  // namespace Protocol

namespace ProtocolInvalid {
    constexpr uint16_t INVALID_START_BYTE_OFFSET = 0xFFFF;     /**< Invalid Start Byte Offset. */
    constexpr uint8_t INVALID_END_BYTE           = 0x00;       /**< Invalid End Byte. */
    constexpr uint16_t INVALID_PAYLOAD_LENGTH    = 0xFFFF;     /**< Invalid Payload Length. */
    constexpr uint8_t INVALID_COMMAND            = 0xFF;       /**< Invalid Command. */
    constexpr uint8_t INVALID_MEMORY_AREA        = 0xFF;       /**< Invalid Memory Area. */
    constexpr uint16_t INVALID_ADDRESS           = 0xFFFF;     /**< Invalid Address. */
    constexpr uint32_t INVALID_CRC               = 0xFFFFFFFF; /**< Invalid CRC. */
    constexpr uint32_t INVALID_UUID              = 0xFFFFFFFF; /**< Invalid CRC. */
}  // namespace ProtocolInvalid

/**
 * @brief Get the offset of the start byte in the buffer.
 *
 * This function searches for the start byte in the given buffer and returns its offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] buffer_size Size of the buffer.
 * @return The offset of the start byte if found, otherwise `INVALID_START_BYTE_OFFSET`.
 */
uint16_t TitaniumProtocol::GetStarByteOffset(uint8_t* buffer, uint16_t buffer_size) {
    auto result = ProtocolInvalid::INVALID_START_BYTE_OFFSET;

    do {
        if (buffer == nullptr) {
            break;
        }

        for (uint16_t index = 0; index < buffer_size; index++) {
            if (buffer[index] == Protocol::START_BYTE) {
                result = index;
                break;
            }
        }
    } while (0);

    return result;
}

/**
 * @brief Retrieve the UUID from the buffer.
 *
 * This function extracts the UUID from the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return The UUID if extraction is successful, otherwise `INVALID_UUID`.
 */
uint32_t TitaniumProtocol::GetUUID(uint8_t* buffer, uint16_t remaining_bytes) {
    auto offset = ProtocolAttributes::UUID_OFFSET;

    if (remaining_bytes - ProtocolAttributes::UUID_SIZE <= 0) {
        return ProtocolErrors::INVALID_UUID;
    }

    return buffer[offset] | (buffer[offset + 1] << 8) | (buffer[offset + 2] << 16) |
           (buffer[offset + 3] << 24);
}

/**
 * @brief Retrieve the payload length from the buffer.
 *
 * This function extracts the payload length from the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return The payload length if extraction is successful, otherwise `INVALID_PAYLOAD_SIZE`.
 */
uint16_t TitaniumProtocol::GetPayloadLength(uint8_t* buffer, uint16_t remaining_bytes) {
    auto offset = ProtocolAttributes::PAYLOAD_LENGTH_OFFSET;

    if (remaining_bytes <= 0) {
        return ProtocolErrors::INVALID_PAYLOAD_SIZE;
    }

    uint8_t payload_length_lsb = buffer[offset];
    uint8_t payload_length_msb = buffer[offset + 1];

    return (payload_length_msb << 8) | payload_length_lsb;
}

/**
 * @brief Retrieve the command from the buffer.
 *
 * This function extracts the command from the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return The command if extraction is successful, otherwise `INVALID_COMMAND`.
 */
command_e TitaniumProtocol::GetCommand(uint8_t* buffer, uint16_t remaining_bytes) {
    if (remaining_bytes <= 0) {
        return static_cast<command_e>(ProtocolErrors::INVALID_COMMAND);
    }
    return static_cast<command_e>(buffer[ProtocolAttributes::COMMAND_OFFSET]);
}

/**
 * @brief Retrieve the memory area from the buffer.
 *
 * This function extracts the memory area from the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return The memory area if extraction is successful, otherwise `INVALID_MEMORY_AREA`.
 */
uint8_t TitaniumProtocol::GetMemoryArea(uint8_t* buffer, uint16_t remaining_bytes) {
    if (remaining_bytes <= 0) {
        return ProtocolErrors::INVALID_MEMORY_AREA;
    }
    return buffer[ProtocolAttributes::MEMORY_AREA_OFFSET];
}

/**
 * @brief Retrieve the address from the buffer.
 *
 * This function extracts the address from the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return The address if extraction is successful, otherwise `INVALID_ADDRESS`.
 */
uint16_t TitaniumProtocol::GetAddress(uint8_t* buffer, uint16_t remaining_bytes) {
    auto offset = ProtocolAttributes::ADDRESS_OFFSET;

    if (remaining_bytes <= 0) {
        return ProtocolErrors::INVALID_ADDRESS;
    }

    uint8_t address_lsb = buffer[offset];
    uint8_t address_msb = buffer[offset + 1];

    return (address_msb << 8) | address_lsb;
}

/**
 * @brief Retrieve the pointer to the payload from the buffer.
 *
 * This function provides a pointer to the payload data in the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return Pointer to the payload if extraction is successful, otherwise `nullptr`.
 */
uint8_t* TitaniumProtocol::GetPayload(uint8_t* buffer, uint16_t remaining_bytes) {
    if (remaining_bytes <= 0) {
        return nullptr;
    }

    return &buffer[ProtocolAttributes::HEADER_OFFSET];
}

/**
 * @brief Retrieve the CRC from the buffer.
 *
 * This function extracts the CRC from the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] payload_size Size of the payload in the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return The CRC if extraction is successful, otherwise `INVALID_CRC`.
 */
uint32_t TitaniumProtocol::GetCRC(uint8_t* buffer, uint16_t payload_size, uint16_t remaining_bytes) {
    auto offset = ProtocolAttributes::HEADER_OFFSET + payload_size;

    if (remaining_bytes <= 0) {
        return ProtocolErrors::INVALID_ADDRESS;
    }

    return buffer[offset] | (buffer[offset + 1] << 8) | (buffer[offset + 2] << 16) |
           (buffer[offset + 3] << 24);
}

/**
 * @brief Retrieve the end byte from the buffer.
 *
 * This function extracts the end byte from the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] payload_size Size of the payload in the message.
 * @param[in] remaining_bytes Number of remaining bytes in the buffer.
 * @return The end byte if extraction is successful, otherwise `INVALID_END_BYTE`.
 */
uint8_t TitaniumProtocol::GetEndByte(uint8_t* buffer, uint16_t payload_size, uint16_t remaining_bytes) {
    auto offset = payload_size + ProtocolAttributes::STATIC_MESSAGE_SIZE;

    if (remaining_bytes <= 0) {
        return ProtocolErrors::INVALID_END_BYTE;
    }

    return buffer[offset];
}

/**
 * @brief Validate the UUID.
 *
 * This function checks if the provided UUID is valid.
 *
 * @param[in] uuid UUID to be validated.
 * @return `ESP_OK` if the UUID is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidateUUID(uint32_t uuid) {
    return uuid != ProtocolInvalid::INVALID_UUID ? ESP_OK : ProtocolErrors::INVALID_END_BYTE;
}

/**
 * @brief Validate if the payload length is within the maximum allowable size.
 *
 * This function checks if the provided payload length does not exceed the maximum size.
 *
 * @param[in] payload_length Payload length to be validated.
 * @return `ESP_OK` if the payload length is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidatePayloadLength(uint16_t payload_length) {
    return payload_length <= Protocol::MAXIMUM_MESSAGE_SIZE ? ESP_OK
                                                            : ProtocolErrors::INVALID_PAYLOAD_SIZE;
}

/**
 * @brief Validate the address.
 *
 * This function checks if the provided address is valid.
 *
 * @param[in] address Address to be validated.
 * @return `ESP_OK` if the address is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidateAddress(uint16_t address) {
    return address == ProtocolInvalid::INVALID_ADDRESS ? ProtocolErrors::INVALID_ADDRESS : ESP_OK;
}

/**
 * @brief Validate the command.
 *
 * This function checks if the provided command is valid.
 *
 * @param[in] command Command to be validated.
 * @return `ESP_OK` if the command is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidateCommand(command_e command) {
    titan_err_t result = ProtocolErrors::INVALID_COMMAND;

    do {
        switch (command) {
            case READ_COMMAND:
            case READ_SECURE_COMMAND:
            case WRITE_COMMAND:
            case READ_RESPONSE_COMMAND:
            case WRITE_SECURE_COMMAND:
            case READ_RESPONSE_SECURE_COMMAND:
            case ACK_COMMAND: {
                result = ESP_OK;
                break;
            }
            default: {
                result = ProtocolErrors::INVALID_COMMAND;
            }
        }
    } while (0);

    return result;
}

/**
 * @brief Validate the memory area.
 *
 * This function checks if the provided memory area is valid.
 *
 * @param[in] memory_area Memory area to be validated.
 * @return `ESP_OK` if the memory area is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidateMemoryArea(uint8_t memory_area) {
    return ESP_OK;
}

/**
 * @brief Validate the payload pointer.
 *
 * This function checks if the provided payload pointer is valid.
 *
 * @param[in] payload Pointer to the payload.
 * @return `ESP_OK` if the payload pointer is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidatePayload(uint8_t* payload) {
    return payload != nullptr ? ESP_OK : ProtocolErrors::INVALID_PAYLOAD_POINTER;
}

/**
 * @brief Validate the CRC of the message.
 *
 * This function checks if the provided CRC matches the calculated CRC of the message.
 *
 * @param[in] crc CRC to be validated.
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] size Size of the buffer.
 * @return `ESP_OK` if the CRC is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidateCRC(uint32_t crc, uint8_t* buffer, uint16_t size) {
    auto calculated_crc = CalculatedCRC32(buffer, size);

    return calculated_crc == crc ? ESP_OK : ProtocolErrors::INVALID_CRC;
}

/**
 * @brief Validate the end byte.
 *
 * This function checks if the provided end byte matches the expected end byte.
 *
 * @param[in] end_byte Received end byte.
 * @return `ESP_OK` if the end byte is valid, otherwise an error code.
 */
titan_err_t TitaniumProtocol::ValidateEndByte(uint8_t end_byte) {
    return end_byte == Protocol::END_BYTE ? ESP_OK : ProtocolErrors::INVALID_END_BYTE;
}

/**
 * @brief Encode the payload length into the buffer.
 *
 * This function encodes the payload length into the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer.
 * @param[in] payload_length Length of the payload.
 * @return `ESP_OK` if encoding is successful, otherwise an error code.
 */
titan_err_t TitaniumProtocol::EncodePayloadLength(uint8_t* buffer, uint16_t payload_length) {
    uint16_t offset = ProtocolAttributes::PAYLOAD_LENGTH_OFFSET;

    if (buffer == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    buffer[offset]     = payload_length & 0xFF;
    buffer[offset + 1] = (payload_length >> 8) & 0xFF;

    return ESP_OK;
}

/**
 * @brief Encode the address into the buffer.
 *
 * This function encodes the address into the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer.
 * @param[in] address Address to be encoded.
 * @return `ESP_OK` if encoding is successful, otherwise an error code.
 */
titan_err_t TitaniumProtocol::EncodeAddress(uint8_t* buffer, uint16_t address) {
    uint16_t offset = ProtocolAttributes::ADDRESS_OFFSET;

    if (buffer == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    buffer[offset]     = address & 0xFF;
    buffer[offset + 1] = (address >> 8) & 0xFF;

    return ESP_OK;
}

/**
 * @brief Encode the CRC into the buffer.
 *
 * This function encodes the CRC into the buffer at the specified offset.
 *
 * @param[in] buffer Pointer to the buffer.
 * @param[in] offset Offset at which the CRC will be encoded.
 * @param[in] crc CRC value to be encoded.
 * @return `ESP_OK` if encoding is successful, otherwise an error code.
 */
titan_err_t TitaniumProtocol::EncodeCRC(uint8_t* buffer, uint16_t offset, uint32_t crc) {
    if (buffer == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    buffer[offset]     = crc & 0xFF;
    buffer[offset + 1] = (crc >> 8) & 0xFF;
    buffer[offset + 2] = (crc >> 16) & 0xFF;
    buffer[offset + 3] = (crc >> 24) & 0xFF;

    return ESP_OK;
}

/**
 * @brief Encode the UUID into the buffer.
 *
 * This function encodes the UUID into the buffer at the predefined offset.
 *
 * @param[in] buffer Pointer to the buffer.
 * @param[in] uuid UUID to be encoded.
 * @return `ESP_OK` if encoding is successful, otherwise an error code.
 */
titan_err_t TitaniumProtocol::EncodeUUID(uint8_t* buffer, uint32_t uuid) {
    uint16_t offset = ProtocolAttributes::UUID_OFFSET;

    if (buffer == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    buffer[offset]     = uuid & 0xFF;
    buffer[offset + 1] = (uuid >> 8) & 0xFF;
    buffer[offset + 2] = (uuid >> 16) & 0xFF;
    buffer[offset + 3] = (uuid >> 24) & 0xFF;

    return ESP_OK;
}

/**
 * @brief Decode the received message and validate its components.
 *
 * This function decodes the message from the buffer and validates its components.
 *
 * @param[in] buffer Pointer to the received message.
 * @param[in] size Size of the received message.
 * @param[out] package Decoded packet payload.
 * @return `ESP_OK` if decoding and validation are successful, otherwise an error code.
 */
titan_err_t TitaniumProtocol::Decode(uint8_t* buffer, size_t size, std::unique_ptr<TitaniumPackage>& package) {
    titan_err_t result      = ESP_OK;
    int16_t remaining_bytes = size;

    do {
        auto start_message_index = this->GetStarByteOffset(buffer, size);

        if (start_message_index == ProtocolInvalid::INVALID_START_BYTE_OFFSET) {
            result = ProtocolErrors::INVALID_START_BYTE;
            break;
        }

        remaining_bytes -= start_message_index;
        if (remaining_bytes <= 0) {
            result = ProtocolErrors::INVALID_START_BYTE;
            break;
        }

        uint8_t* start_message_pointer = buffer + start_message_index;

        remaining_bytes -= ProtocolAttributes::UUID_SIZE;
        auto uuid = this->GetUUID(start_message_pointer, remaining_bytes);
        if (this->ValidateUUID(uuid) != ESP_OK) {
            result = ProtocolErrors::INVALID_UUID;
            break;
        }

        remaining_bytes -= ProtocolAttributes::PAYLOAD_LENGTH_SIZE;
        auto payload_length = this->GetPayloadLength(start_message_pointer, remaining_bytes);
        if (this->ValidatePayloadLength(payload_length) != ESP_OK) {
            result = ProtocolErrors::INVALID_PAYLOAD_SIZE;
            break;
        }

        remaining_bytes -= ProtocolAttributes::COMMAND_SIZE;
        auto command = this->GetCommand(start_message_pointer, remaining_bytes);
        if (this->ValidateCommand(command) != ESP_OK) {
            result = ProtocolErrors::INVALID_COMMAND;
            break;
        }

        remaining_bytes -= ProtocolAttributes::MEMORY_AREA_SIZE;
        auto memory_area = this->GetMemoryArea(start_message_pointer, remaining_bytes);
        if (this->ValidateMemoryArea(memory_area) != ESP_OK) {
            result = ProtocolErrors::INVALID_MEMORY_AREA;
            break;
        }

        remaining_bytes -= ProtocolAttributes::ADDRESS_SIZE;
        auto address = this->GetAddress(start_message_pointer, remaining_bytes);
        if (this->ValidateAddress(address) != ESP_OK) {
            result = ProtocolErrors::INVALID_ADDRESS;
            break;
        }

        remaining_bytes -= payload_length;
        auto payload = this->GetPayload(start_message_pointer, remaining_bytes);
        if (this->ValidatePayload(payload) != ESP_OK) {
            result = ProtocolErrors::INVALID_PAYLOAD_POINTER;
            break;
        }

        remaining_bytes -= ProtocolAttributes::CRC_SIZE;
        auto crc32  = this->GetCRC(start_message_pointer, payload_length, remaining_bytes);
        auto offset = payload_length + ProtocolAttributes::HEADER_OFFSET;
        if (this->ValidateCRC(crc32, start_message_pointer, offset) != ESP_OK) {
            result = ProtocolErrors::INVALID_CRC;
            break;
        }

        remaining_bytes -= ProtocolAttributes::END_BYTE_SIZE;
        auto end_byte = this->GetEndByte(start_message_pointer, payload_length, remaining_bytes);
        if (this->ValidateEndByte(end_byte) != ESP_OK) {
            result = ProtocolErrors::INVALID_END_BYTE;
            break;
        }

        if (result == ESP_OK) {
            package.reset(
                new TitaniumPackage(payload_length, uuid, address, command, memory_area, payload));
        }
    } while (0);

    return result;
}

/**
 * @brief Encode the given TitaniumPackage into the buffer.
 *
 * This function encodes the provided TitaniumPackage into the buffer.
 *
 * @param[in] package Unique pointer to the TitaniumPackage to encode.
 * @param[out] buffer Pointer to the buffer where the encoded package will be stored.
 * @param[in] size Size of the buffer.
 * @return Number of bytes written into the buffer.
 */
uint16_t TitaniumProtocol::Encode(std::unique_ptr<TitaniumPackage>& package, uint8_t* buffer, uint16_t size) {
    uint16_t result = 0;

    do {
        if (buffer == nullptr) {
            break;
        }

        uint16_t expected_size = (package.get()->size() +
                                  ProtocolAttributes::STATIC_MESSAGE_SIZE +
                                  ProtocolAttributes::END_BYTE_SIZE);

        if (size < (package.get()->size() +
                    ProtocolAttributes::STATIC_MESSAGE_SIZE +
                    ProtocolAttributes::END_BYTE_SIZE)) {
            break;
        }

        buffer[ProtocolAttributes::START_BYTE_OFFSET]  = Protocol::START_BYTE;
        buffer[ProtocolAttributes::COMMAND_OFFSET]     = package.get()->command();
        buffer[ProtocolAttributes::MEMORY_AREA_OFFSET] = package.get()->memory_area();

        auto end_byte_position    = package.get()->size() + ProtocolAttributes::STATIC_MESSAGE_SIZE;
        buffer[end_byte_position] = Protocol::END_BYTE;

        package.get()->Consume(&buffer[ProtocolAttributes::HEADER_OFFSET]);

        this->EncodeUUID(buffer, package.get()->uuid());
        this->EncodePayloadLength(buffer, package.get()->size());
        this->EncodeAddress(buffer, package.get()->address());

        uint16_t crc_offset = ProtocolAttributes::HEADER_OFFSET + package.get()->size();
        this->EncodeCRC(buffer, crc_offset, CalculatedCRC32(buffer, crc_offset));

        result = expected_size;
    } while (0);

    return result;
}