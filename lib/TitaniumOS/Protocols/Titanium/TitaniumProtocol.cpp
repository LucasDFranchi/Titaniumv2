#include "TitaniumProtocol.h"

#include "./Utils/CRCUtils.h"
#include "HAL/memory/MemoryHandlers.h"

namespace Protocol {
    constexpr uint8_t START_BYTE                = 2;    /**< Start byte of the message. */
    constexpr uint8_t START_BYTE_OFFSET         = 0;    /**< Offset position of the start byte. */
    constexpr uint8_t PAYLOAD_LENGTH_LSB_OFFSET = 1;    /**< Offset of the least significant byte of payload length. */
    constexpr uint8_t PAYLOAD_LENGTH_MSB_OFFSET = 2;    /**< Offset of the most significant byte of payload length. */
    constexpr uint8_t COMMAND_OFFSET            = 3;    /**< Offset of the command byte in the message. */
    constexpr uint8_t MEMORY_AREA_OFFSET        = 4;    /**< Offset of the memory area byte in the message. */
    constexpr uint8_t HEADER_OFFSET             = 5;    /**< Offset of the message header. */
    constexpr uint8_t END_BYTE                  = 3;    /**< End byte of the message. */
    constexpr uint8_t STATIC_MESSAGE_SIZE       = 9;    /**< Size of the static part of the message. */
    constexpr uint8_t CRC_FIRST_BYTE            = 0;    /**< First byte of the CRC. */
    constexpr uint8_t CRC_SECOND_BYTE           = 1;    /**< Second byte of the CRC. */
    constexpr uint8_t CRC_THIRD_BYTE            = 2;    /**< Third byte of the CRC. */
    constexpr uint8_t CRC_FOURTH_BYTE           = 3;    /**< Fourth byte of the CRC. */
    constexpr uint16_t MAXIMUM_MESSAGE_SIZE     = 1024; /**< Maximum size of a message. */
}  // namespace Protocol

/**
 * @brief Get the offset of the start byte in the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] buffer_size Size of the buffer.
 * @return std::pair<int16_t, esp_err_t> Pair containing the offset of the start
 * byte (or an error code if not found) and an error code indicating the result
 * of the operation.
 */
std::pair<int16_t, esp_err_t> TitaniumProtocol::GetStarByteOffset(
    uint8_t* buffer, uint16_t buffer_size) {
    std::pair<uint16_t, esp_err_t> result =
        std::make_pair(ProtocolErrors::INVALID_PAYLOAD_SIZE,
                       ProtocolErrors::INVALID_START_BYTE);

    do {
        if (buffer == nullptr) {
            break;
        }

        for (uint16_t i = 0; i < buffer_size; i++) {
            if (buffer[i] == Protocol::START_BYTE) {
                result = std::make_pair(i, ESP_OK);
            }
        }
    } while (0);

    return result;
}

/**
 * @brief Retrieve the payload length from the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @return std::pair<int16_t, esp_err_t> Pair containing the payload length and
 * an error code indicating the result of the operation.
 */
std::pair<int16_t, esp_err_t> TitaniumProtocol::GetPayloadLength(
    uint8_t* buffer) {
    std::pair<uint16_t, esp_err_t> result =
        std::make_pair(ProtocolErrors::INVALID_PAYLOAD_SIZE, ESP_ERR_NO_MEM);

    do {
        if (buffer == nullptr) {
            break;
        }

        uint8_t payload_length_lsb =
            buffer[Protocol::PAYLOAD_LENGTH_LSB_OFFSET];
        uint8_t payload_length_msb =
            buffer[Protocol::PAYLOAD_LENGTH_MSB_OFFSET];
        uint16_t payload_length =
            (payload_length_msb << 8) | payload_length_lsb;

        result = std::make_pair(payload_length, ESP_OK);

    } while (0);

    return result;
}

/**
 * @brief Retrieve the command from the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @return std::pair<command_e, esp_err_t> Pair containing the command and an
 * error code indicating the result of the operation.
 */
std::pair<command_e, esp_err_t> TitaniumProtocol::GetCommand(uint8_t* buffer) {
    std::pair<command_e, esp_err_t> result =
        std::make_pair(INVALID_OPERATION, ESP_ERR_NO_MEM);

    do {
        if (buffer == nullptr) {
            break;
        }

        command_e command =
            static_cast<command_e>(buffer[Protocol::COMMAND_OFFSET]);

        result = std::make_pair(command, ESP_OK);

    } while (0);

    return result;
}

/**
 * @brief Retrieve the memory area from the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @return std::pair<uint8_t, esp_err_t> Pair containing the memory area and an
 * error code indicating the result of the operation.
 */
std::pair<uint8_t, esp_err_t> TitaniumProtocol::GetMemoryArea(uint8_t* buffer) {
    std::pair<uint8_t, esp_err_t> result =
        std::make_pair(ProtocolErrors::INVALID_MEMORY_AREA, ESP_ERR_NO_MEM);

    do {
        if (buffer == nullptr) {
            break;
        }

        uint8_t memory_area = buffer[Protocol::MEMORY_AREA_OFFSET];

        result = std::make_pair(memory_area, ESP_OK);

    } while (0);

    return result;
}

/**
 * @brief Retrieve the pointer to the payload from the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @return std::pair<uint8_t*, esp_err_t> Pair containing the pointer to the
 * payload and an error code indicating the result of the operation.
 */
std::pair<uint8_t*, esp_err_t> TitaniumProtocol::GetPayload(uint8_t* buffer) {
    std::pair<uint8_t*, esp_err_t> result =
        std::make_pair(nullptr, ESP_ERR_NO_MEM);

    do {
        if (buffer == nullptr) {
            break;
        }

        uint8_t* payload = &buffer[Protocol::HEADER_OFFSET];

        result = std::make_pair(payload, ESP_OK);

    } while (0);

    return result;
}

/**
 * @brief Retrieve the CRC from the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] payload_size Size of the payload in the message.
 * @return std::pair<uint32_t, esp_err_t> Pair containing the CRC and an error
 * code indicating the result of the operation.
 */
std::pair<uint32_t, esp_err_t> TitaniumProtocol::GetCRC(uint8_t* buffer,
                                                        uint16_t payload_size) {
    std::pair<uint32_t, esp_err_t> result =
        std::make_pair(ProtocolErrors::INVALID_CRC, ESP_ERR_NO_MEM);
    uint32_t crc32 = 0x00;

    do {
        if (buffer == nullptr) {
            break;
        }

        uint8_t* start_crc = buffer + Protocol::HEADER_OFFSET + payload_size;

        crc32 = start_crc[Protocol::CRC_FIRST_BYTE] |
                (start_crc[Protocol::CRC_SECOND_BYTE] << 8) |
                (start_crc[Protocol::CRC_THIRD_BYTE] << 16) |
                (start_crc[Protocol::CRC_FOURTH_BYTE] << 24);

        result = std::make_pair(crc32, ESP_OK);

    } while (0);

    return result;
}

/**
 * @brief Retrieve the end byte from the buffer.
 *
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] payload_size Size of the payload in the message.
 * @return std::pair<uint8_t, esp_err_t> Pair containing the end byte and an error code indicating the result of the operation.
 */
std::pair<uint8_t, esp_err_t> TitaniumProtocol::GetEndByte(
    uint8_t* buffer, uint16_t payload_size) {
    std::pair<uint32_t, esp_err_t> result =
        std::make_pair(ProtocolErrors::INVALID_END_BYTE, ESP_ERR_NO_MEM);

    do {
        if (buffer == nullptr) {
            break;
        }

        uint8_t end_byte = static_cast<uint8_t>(
            *(buffer + payload_size + Protocol::STATIC_MESSAGE_SIZE));

        result = std::make_pair(end_byte, ESP_OK);

    } while (0);

    return result;
}

/**
 * @brief Validate if the payload length is within the maximum allowable size.
 *
 * @param[in] payload_length Data length to be validated.
 * @return esp_err_t Error code indicating the result of the validation.
 */
esp_err_t TitaniumProtocol::ValidatePayloadLength(uint16_t payload_length) {
    return payload_length <= Protocol::MAXIMUM_MESSAGE_SIZE
               ? ESP_OK
               : ProtocolErrors::INVALID_PAYLOAD_SIZE;
}

/**
 * @brief Validate if the command is one of the expected types.
 *
 * @param[in] command Command to be validated.
 * @return esp_err_t Error code indicating the result of the validation.
 */
esp_err_t TitaniumProtocol::ValidateCommand(command_e command) {
    esp_err_t result = ProtocolErrors::INVALID_COMMAND;

    do {
        if (command == READ_COMMAND) {
            result = ESP_OK;
            break;
        }
        if (command == WRITE_COMMAND) {
            result = ESP_OK;
            break;
        }
    } while (0);

    return result;
}

/**
 * @brief Validate if the memory area is valid.
 *
 * @param[in] memory_area Memory area to be validated.
 * @return esp_err_t Error code indicating the result of the validation.
 */
esp_err_t TitaniumProtocol::ValidateMemoryArea(uint8_t memory_area) {
    return ESP_OK;
}

/**
 * @brief Validate the payload pointer.
 *
 * @param[in] payload Pointer to the payload.
 * @return esp_err_t Error code indicating the result of the validation.
 */
esp_err_t TitaniumProtocol::ValidatePayload(uint8_t* payload) {
    return payload != nullptr ? ESP_OK
                              : ProtocolErrors::INVALID_PAYLOAD_POINTER;
}

/**
 * @brief Validate the CRC of the message.
 *
 * @param[in] crc CRC to be validated.
 * @param[in] buffer Pointer to the buffer containing the message.
 * @param[in] size Size of the buffer.
 * @return esp_err_t Error code indicating the result of the validation.
 */
esp_err_t TitaniumProtocol::ValidateCRC(uint32_t crc, uint8_t* buffer,
                                        uint16_t size) {
    auto calculated_crc = CalculatedCRC32(buffer, size);

    return calculated_crc == crc ? ESP_OK : ProtocolErrors::INVALID_CRC;
}

/**
 * @brief Validate if the received end byte matches the expected end byte.
 *
 * @param[in] end_byte Received end byte.
 * @return esp_err_t Error code indicating the result of the validation.
 */
esp_err_t TitaniumProtocol::ValidateEndByte(uint8_t end_byte) {
    return end_byte == Protocol::END_BYTE ? ESP_OK
                                          : ProtocolErrors::INVALID_END_BYTE;
}

/**
 * @brief Encode the payload length into the buffer.
 *
 * @param[in] buffer Pointer to the buffer.
 * @param[in] payload_length Length of the payload.
 */
void TitaniumProtocol::EncodePayloadLength(uint8_t* buffer,
                                           uint16_t payload_length) {
    buffer[Protocol::PAYLOAD_LENGTH_LSB_OFFSET] = payload_length & 0xFF;
    buffer[Protocol::PAYLOAD_LENGTH_MSB_OFFSET] = (payload_length >> 8) & 0xFF;
}

/**
 * @brief Encode the CRC into the buffer.
 *
 * @param[in] buffer Pointer to the buffer.
 * @param[in] crc CRC value to be encoded.
 */
void TitaniumProtocol::EncodeCRC(uint8_t* buffer, uint32_t crc) {
    buffer[Protocol::CRC_FIRST_BYTE]  = crc & 0xFF;
    buffer[Protocol::CRC_SECOND_BYTE] = (crc >> 8) & 0xFF;
    buffer[Protocol::CRC_THIRD_BYTE]  = (crc >> 16) & 0xFF;
    buffer[Protocol::CRC_FOURTH_BYTE] = (crc >> 24) & 0xFF;
}

/**
 * @brief Decode the received message and validate its components.
 *
 * @param[in] buffer Pointer to the received message.
 * @param[in] size Size of the received message.
 * @param[out] package Decoded packet payload.
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t TitaniumProtocol::Decode(uint8_t* buffer, size_t size,
                                   std::unique_ptr<TitaniumPackage>& package) {
    esp_err_t result = ESP_OK;
    do {
        auto start_message_index = this->GetStarByteOffset(buffer, size);

        if (start_message_index.second == ProtocolErrors::INVALID_START_BYTE) {
            result = start_message_index.second;
            break;
        }

        uint8_t* start_message_pointer = buffer + start_message_index.second;

        auto payload_length = this->GetPayloadLength(start_message_pointer);
        if (payload_length.second == ESP_ERR_NO_MEM) {
            result = payload_length.second;
            break;
        }
        auto command = this->GetCommand(start_message_pointer);
        if (command.second == ESP_ERR_NO_MEM) {
            result = command.second;
            break;
        }
        auto memory_area = this->GetMemoryArea(start_message_pointer);
        if (memory_area.second == ESP_ERR_NO_MEM) {
            result = memory_area.second;
            break;
        }
        auto payload = this->GetPayload(start_message_pointer);
        if (payload.second == ESP_ERR_NO_MEM) {
            result = payload.second;
            break;
        }
        auto crc32 = this->GetCRC(start_message_pointer, payload_length.first);
        if (crc32.second == ESP_ERR_NO_MEM) {
            result = crc32.second;
            break;
        }
        auto end_byte =
            this->GetEndByte(start_message_pointer, payload_length.first);
        if (end_byte.second == ESP_ERR_NO_MEM) {
            result = end_byte.second;
            break;
        }

        result = this->ValidatePayloadLength(payload_length.first);
        if (result != ESP_OK) {
            break;
        }
        result = this->ValidateCommand(command.first);
        if (result != ESP_OK) {
            break;
        }
        result = this->ValidateMemoryArea(memory_area.first);
        if (result != ESP_OK) {
            break;
        }
        result = this->ValidatePayload(payload.first);
        if (result != ESP_OK) {
            break;
        }
        result =
            this->ValidateCRC(crc32.first, start_message_pointer,
                              payload_length.first + Protocol::HEADER_OFFSET);
        if (result != ESP_OK) {
            break;
        }
        result = this->ValidateEndByte(end_byte.first);
        if (result != ESP_OK) {
            break;
        }

        if (result == ESP_OK) {
            package.reset(new TitaniumPackage(payload_length.first,
                                              command.first, memory_area.first,
                                              payload.first));
        }
    } while (0);

    return result;
}

/**
 * @brief Encode the given TitaniumPackage into the buffer.
 *
 * @param[in] package Unique pointer to the TitaniumPackage to encode.
 * @param[out] buffer Pointer to the buffer where the encoded package will be stored.
 * @param[in] size Size of the buffer.
 * @return uint16_t Amount of bytes written in the buffer.
 */
uint16_t TitaniumProtocol::Encode(std::unique_ptr<TitaniumPackage>& package,
                                  uint8_t* buffer, uint16_t size) {
    uint16_t result     = 0;
    uint16_t crc_offset = Protocol::HEADER_OFFSET + package.get()->size();

    do {
        if (buffer == nullptr) {
            break;
        }

        uint16_t expected_size = (package.get()->size() + Protocol::STATIC_MESSAGE_SIZE + 1);

        if (size < (package.get()->size() + Protocol::STATIC_MESSAGE_SIZE + 1)) {
            break;
        }

        buffer[Protocol::START_BYTE_OFFSET]  = Protocol::START_BYTE;
        buffer[Protocol::COMMAND_OFFSET]     = package.get()->command();
        buffer[Protocol::MEMORY_AREA_OFFSET] = package.get()->memory_area();
        buffer[package.get()->size() + Protocol::STATIC_MESSAGE_SIZE] =
            Protocol::END_BYTE;

        package.get()->Consume(&buffer[Protocol::HEADER_OFFSET]);

        this->EncodePayloadLength(buffer, package.get()->size());
        this->EncodeCRC(&buffer[crc_offset], CalculatedCRC32(buffer, crc_offset));

        result = expected_size;
    } while (0);

    return result;
}