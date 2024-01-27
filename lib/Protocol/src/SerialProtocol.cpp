#include "SerialProtocol.h"

#include "ByteConverterUtils.h"
#include "CRCUtils.h"
#include "MemoryUtils.h"

constexpr uint8_t START_BYTE_POSITION = 0x00;
constexpr uint8_t DATA_LENGTH_POSITION = 0x01;
constexpr uint8_t START_COMMAND_POSITION = 0x03;
constexpr uint8_t START_MEMORY_AREA_POSITION = 0x04;
constexpr uint8_t END_BYTE_POSITION = 0x00;
constexpr uint8_t HEADER_SIZE = 0x05;
constexpr uint8_t END_CRC_POSITION_IN_FIXED = 0x09;
constexpr uint8_t FIXED_PROTOCOL_SIZE = 0x0A;

constexpr uint8_t START_BYTE = 0x02;
constexpr uint8_t END_BYTE = 0x03;
constexpr uint16_t MAXIMUM_MESSAGE_SIZE = 1024;

/**
 * @brief Processes an incoming message by extracting relevant information and
 * validating it.
 *
 * @param bytes A pointer to an array of bytes that represents the incoming
 * message.
 * @param size The size of the 'bytes' array, which is the number of elements in
 * the array.
 *
 * @return An 'esp_err_t' value, which is a type defined in the ESP-IDF
 * framework.
 */
esp_err_t SerialProtocol::ProcessIncomingMessage(uint8_t *bytes, size_t size) {
    esp_err_t result = ESP_FAIL;

    this->bytes = bytes;
    this->incoming_message_.start_byte = this->bytes[START_BYTE_POSITION];
    this->incoming_message_.data_length =
        ConvertBytesToUInt16(&this->bytes[DATA_LENGTH_POSITION]);
    this->incoming_message_.command = this->bytes[START_COMMAND_POSITION];
    this->incoming_message_.memory_area =
        this->bytes[START_MEMORY_AREA_POSITION];

    size_t crc_position = HEADER_SIZE + this->incoming_message_.data_length;
    size_t end_position =
        END_CRC_POSITION_IN_FIXED + this->incoming_message_.data_length;

    this->incoming_message_.crc32 =
        ConvertBytesToUInt32(&this->bytes[crc_position]);
    this->incoming_message_.end_byte = this->bytes[end_position];

    result = this->ValidateIncomingMessage(this->bytes);

    if (result == ESP_OK) {
        this->incoming_message_.pointer_data = &this->bytes[HEADER_SIZE];
    }

    return result;
}

/**
 * @brief Validates an incoming message.
 *
 * @param bytes A pointer to an array of bytes that represents the incoming
 * message.
 *
 * @return An 'esp_err_t' value, which is a type defined in the ESP-IDF
 * framework.
 */
esp_err_t SerialProtocol::ValidateIncomingMessage(uint8_t *bytes) {
    esp_err_t result = ESP_FAIL;

    do {
        result = ValidateStartByte_(this->incoming_message_.start_byte);
        if (result != ESP_OK) break;
        result = ValidateDataLength_(this->incoming_message_.data_length);
        if (result != ESP_OK) break;
        result = ValidateCommand_(this->incoming_message_.command);
        if (result != ESP_OK) break;
        result = ValidateCRC_(&this->incoming_message_, bytes);
        if (result != ESP_OK) break;
        result = ValidateEndByte_(this->incoming_message_.end_byte);
        if (result != ESP_OK) break;

    } while (0);

    return result;
}

/**
 * @brief Generates a response message based on the provided parameters.
 *
 * This function constructs a response message using the given response data,
 * size, and other information from the incoming message. It sets the start
 * byte, data length, command, memory area, data pointer, calculates CRC32, and
 * sets the end byte.
 *
 * @param response_data A pointer to an array of bytes representing the response
 * data.
 * @param size The size of the response data.
 * @param memory_area The memory area associated with the response message.
 *
 * @return The total size of the generated response message.
 */
uint16_t SerialProtocol::GenerateResponseMessage(uint8_t *response_data,
                                                 uint16_t size,
                                                 uint8_t memory_area) {
    this->sending_message_.start_byte = START_BYTE;
    this->sending_message_.data_length = ConvertBytesToUInt16((uint8_t *)&size);
    this->sending_message_.command = this->bytes[START_COMMAND_POSITION];
    this->sending_message_.memory_area =
        this->bytes[START_MEMORY_AREA_POSITION];
    this->sending_message_.pointer_data = response_data;
    this->sending_message_.crc32 =
        CalculatedCRC32(&this->sending_message_.start_byte,
                        this->sending_message_.data_length + HEADER_SIZE);
    this->sending_message_.end_byte = END_BYTE;

    return size + FIXED_PROTOCOL_SIZE;
}

/**
 * @brief Validates the start byte of an incoming message.
 *
 * @param start_byte The start byte of the incoming message.
 *
 * @return ESP_OK if the start byte is valid, ESP_FAIL otherwise.
 */
esp_err_t SerialProtocol::ValidateStartByte_(uint8_t start_byte) {
    return start_byte == START_BYTE ? ESP_OK : ESP_FAIL;
}

/**
 * @brief Validates the data length of an incoming message.
 *
 * @param data_length The data length of the incoming message.
 *
 * @return ESP_OK if the data length is valid, ESP_ERR_INVALID_SIZE otherwise.
 */
esp_err_t SerialProtocol::ValidateDataLength_(uint16_t data_length) {
    return data_length <= MAXIMUM_MESSAGE_SIZE ? ESP_OK : ESP_ERR_INVALID_SIZE;
}

/**
 * @brief Validates the command of an incoming message.
 *
 * @param command The command byte of the incoming message.
 *
 * @return ESP_OK if the command is valid, ESP_ERR_INVALID_ARG otherwise.
 */
esp_err_t SerialProtocol::ValidateCommand_(uint8_t command) {
    esp_err_t result = ESP_ERR_INVALID_ARG;

    do {
        if (command == 'R') {
            result = ESP_OK;
            break;
        }
        if (command == 'W') {
            result = ESP_OK;
            break;
        }
    } while (0);

    return result;
}

/**
 * @brief Validates the CRC of an incoming message.
 *
 * @param incoming_message A pointer to the ProtocolData_st structure
 * representing the incoming message.
 * @param bytes A pointer to an array of bytes that represents the incoming
 * message.
 *
 * @return ESP_OK if the CRC is valid, ESP_ERR_INVALID_CRC otherwise.
 */
esp_err_t SerialProtocol::ValidateCRC_(const ProtocolData_st *incoming_message,
                                       uint8_t *bytes) {
    uint32_t calculated_crc =
        CalculatedCRC32(bytes, incoming_message->data_length + HEADER_SIZE);
    return incoming_message->crc32 == calculated_crc ? ESP_OK
                                                     : ESP_ERR_INVALID_CRC;
}

/**
 * @brief Validates the end byte of an incoming message.
 *
 * @param end_byte The end byte of the incoming message.
 *
 * @return ESP_OK if the end byte is valid, ESP_FAIL otherwise.
 */
esp_err_t SerialProtocol::ValidateEndByte_(uint8_t end_byte) {
    return end_byte == END_BYTE ? ESP_OK : ESP_FAIL;
}

/**
 * @brief Gets the incoming message.
 *
 * @return A constant reference to the ProtocolData_st structure representing
 * the incoming message.
 */
ProtocolData_st SerialProtocol::GetMessageIncoming() const & {
    return this->incoming_message_;
}

/**
 * @brief Gets the sending message.
 *
 * @return A constant reference to the ProtocolData_st structure representing
 * the incoming message.
 */
ProtocolData_st SerialProtocol::GetMessageSending() const & {
    return this->sending_message_;
}
