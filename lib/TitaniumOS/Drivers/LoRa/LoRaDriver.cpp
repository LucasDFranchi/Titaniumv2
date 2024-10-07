#include "LoRaDriver.h"

#include "HAL/gpio/GPIOManager.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

namespace Registers {
    constexpr uint8_t FIFO                 = 0x00;
    constexpr uint8_t OP_MODE              = 0x01;
    constexpr uint8_t FRF_MSB              = 0x06;
    constexpr uint8_t FRF_MID              = 0x07;
    constexpr uint8_t FRF_LSB              = 0x08;
    constexpr uint8_t PA_CONFIG            = 0x09;
    constexpr uint8_t LNA                  = 0x0c;
    constexpr uint8_t FIFO_ADDR_PTR        = 0x0d;
    constexpr uint8_t FIFO_TX_BASE_ADDR    = 0x0e;
    constexpr uint8_t FIFO_RX_BASE_ADDR    = 0x0f;
    constexpr uint8_t FIFO_RX_CURRENT_ADDR = 0x10;
    constexpr uint8_t IRQ_FLAGS            = 0x12;
    constexpr uint8_t RX_NB_BYTES          = 0x13;
    constexpr uint8_t MODEM_STATUS         = 0x13;
    constexpr uint8_t PKT_SNR_VALUE        = 0x19;
    constexpr uint8_t PKT_RSSI_VALUE       = 0x1a;
    constexpr uint8_t MODEM_CONFIG_1       = 0x1d;
    constexpr uint8_t MODEM_CONFIG_2       = 0x1e;
    constexpr uint8_t PREAMBLE_MSB         = 0x20;
    constexpr uint8_t PREAMBLE_LSB         = 0x21;
    constexpr uint8_t PAYLOAD_LENGTH       = 0x22;
    constexpr uint8_t MODEM_CONFIG_3       = 0x26;
    constexpr uint8_t RSSI_WIDEBAND        = 0x2c;
    constexpr uint8_t DETECTION_OPTIMIZE   = 0x31;
    constexpr uint8_t DETECTION_THRESHOLD  = 0x37;
    constexpr uint8_t SYNC_WORD            = 0x39;
    constexpr uint8_t DIO_MAPPING_1        = 0x40;
    constexpr uint8_t VERSION              = 0x42;
}  // namespace Registers

namespace TransceiverModes {
    constexpr uint8_t LONG_RANGE_MODE = 0x80;
    constexpr uint8_t SLEEP           = 0x00;
    constexpr uint8_t STDBY           = 0x01;
    constexpr uint8_t TX              = 0x03;
    constexpr uint8_t RX_CONTINUOUS   = 0x05;
    constexpr uint8_t RX_SINGLE       = 0x06;
}  // namespace TransceiverModes

namespace PAConfiguration {
    constexpr uint8_t PA_BOOST               = 0x80;
    constexpr uint8_t PA_OUTPUT_RFO_PIN      = 0x80;
    constexpr uint8_t PA_OUTPUT_PA_BOOST_PIN = 0x80;
}  // namespace PAConfiguration

namespace IRQ {
    constexpr uint8_t CAD_DETECTED           = 0x01;
    constexpr uint8_t FHSS_CHANGE_CHANNEL    = 0x02;
    constexpr uint8_t CAD_DONE               = 0x04;
    constexpr uint8_t TX_DONE_MASK           = 0x08;
    constexpr uint8_t VALID_HEADER           = 0x10;
    constexpr uint8_t PAYLOAD_CRC_ERROR_MASK = 0x20;
    constexpr uint8_t RX_DONE_MASK           = 0x40;
    constexpr uint8_t RX_TIMEOUT             = 0x80;
}  // namespace IRQ

namespace Driver {
    constexpr uint8_t VERSION       = 0x12;
    constexpr uint8_t WRITE_COMMAND = 0x80;
}  // namespace Driver

/**
 * @brief Writes a buffer of raw bytes to the LoRa transceiver.
 *
 * This function sends a packet containing raw bytes to the LoRa transceiver.
 * It first checks if the input buffer pointer is valid, then calls SendPacket
 * to initiate the transmission. If successful, it sets the result to ESP_OK.
 *
 * @param[in] raw_bytes Pointer to the buffer containing data to transmit.
 * @param[in] size Size of the data buffer in bytes.
 * @return ESP_OK if the transmission is successful, otherwise an error code.
 */
titan_err_t LoRaDriver::Write(uint8_t *raw_bytes, uint16_t size) {
    auto result = Error::UNKNOW_FAIL;

    do {
        if (raw_bytes == nullptr) {
            break;
        }
        this->SendPacket(raw_bytes, size);

        result = ESP_OK;

    } while (0);

    return result;
}

/**
 * @brief Reads a packet of raw bytes from the LoRa transceiver.
 *
 * This function attempts to receive a packet of raw bytes from the LoRa
 * transceiver. It first checks if the input buffer pointer is valid and then
 * sets the transceiver to receiver mode. If there is data available in the
 * receiver buffer, it reads the packet into the provided buffer up to the
 * size specified by _buffer_size.
 *
 * @param[out] raw_bytes Pointer to the buffer where received data will be stored.
 * @return The number of bytes received, or 0 if no data is received or on error.
 */
uint16_t LoRaDriver::Read(uint8_t *raw_bytes) {
    auto received_bytes = 0;

    do {
        if (raw_bytes == nullptr) {
            break;
        }

        this->SetReceiverMode();

        if (this->isDataInReceiver()) {
            received_bytes = this->ReceivePacket(raw_bytes, this->_buffer_size);
        }

    } while (0);

    return received_bytes;
}

/**
 * @brief Initializes the LoRa driver.
 *
 * This function initializes the LoRa driver by configuring SPI and GPIO
 * managers, performing a reset, validating the version, and setting initial
 * configuration.
 *
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
titan_err_t LoRaDriver::Initialize(void) {
    auto result = ESP_OK;

    this->spi_manager  = SPIManager::GetInstance();
    this->gpio_manager = GPIOManager::GetInstance();

    this->Reset();

    result += this->ValidateVersion();
    ESP_ERROR_CHECK(result);

    this->SetSleepMode();
    result += this->WriteRegister(Registers::FIFO_RX_BASE_ADDR, 0);
    result += this->WriteRegister(Registers::FIFO_TX_BASE_ADDR, 0);
    result += this->WriteRegister(Registers::LNA,
                                  this->ReadRegister(Registers::LNA) | 0x03);
    result += this->WriteRegister(Registers::MODEM_CONFIG_3, 0x04);
    this->SetTxPower(17);

    this->SetIdleMode();

    return result;
}

/**
 * @brief Resets the LoRa transceiver.
 *
 * This function resets the LoRa transceiver by toggling the reset pin.
 */
void LoRaDriver::Reset(void) {
    gpio_manager->WriteGPIO(LORA_RST, LOW);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_manager->WriteGPIO(LORA_RST, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
}

/**
 * @brief Sets the LoRa transceiver to explicit header mode.
 *
 * This function sets the transceiver to explicit header mode by updating the
 * MODEM_CONFIG_1 register.
 *
 * @see SetImplicitHeaderMode
 */
void LoRaDriver::SetExplicitHeaderMode(void) {
    this->_implicit = false;
    this->WriteRegister(Registers::MODEM_CONFIG_1,
                        this->ReadRegister(Registers::MODEM_CONFIG_1) & 0xfe);
}

/**
 * @brief Sets the LoRa transceiver to implicit header mode with a specified
 * payload size.
 *
 * This function sets the transceiver to implicit header mode by updating the
 * MODEM_CONFIG_1 register and also sets the payload length.
 *
 * @param[in] size Size of the payload in implicit header mode.
 * @see SetExplicitHeaderMode
 */
void LoRaDriver::SetImplicitHeaderMode(uint8_t size) {
    this->_implicit = true;
    this->WriteRegister(Registers::MODEM_CONFIG_1,
                        this->ReadRegister(Registers::MODEM_CONFIG_1) | 0x01);
    this->WriteRegister(Registers::PAYLOAD_LENGTH, size);
}

/**
 * @brief Sets the LoRa transceiver to sleep mode.
 *
 * This function sets the transceiver to sleep mode by updating the OP_MODE
 * register.
 *
 * @see SetIdleMode
 */
void LoRaDriver::SetSleepMode(void) {
    this->WriteRegister(Registers::OP_MODE, TransceiverModes::LONG_RANGE_MODE |
                                                TransceiverModes::SLEEP);

    this->_reciever_mode = false;
}

/**
 * @brief Sets the LoRa transceiver to idle mode.
 *
 * This function sets the transceiver to idle mode by updating the OP_MODE
 * register.
 *
 * @see SetSleepMode
 */
void LoRaDriver::SetIdleMode(void) {
    this->WriteRegister(Registers::OP_MODE, TransceiverModes::LONG_RANGE_MODE |
                                                TransceiverModes::STDBY);
    this->_reciever_mode = false;
}

/**
 * @brief Sets the LoRa transceiver to continuous receiver mode.
 *
 * This function sets the transceiver to continuous receiver mode by updating
 * the OP_MODE register. If the receiver mode was not set before, it sets the
 * internal flag.
 *
 * @see SetIdleMode
 */
void LoRaDriver::SetReceiverMode(void) {
    if (!this->_reciever_mode) {
        this->WriteRegister(
            Registers::OP_MODE,
            TransceiverModes::LONG_RANGE_MODE | TransceiverModes::RX_CONTINUOUS);
        this->_reciever_mode = true;
    }
}

/**
 * @brief Sets the transmit power of the LoRa transceiver.
 *
 * This function sets the transmit power by updating the PA_CONFIG register.
 * The transmit power level is clamped to the range [2, 17].
 *
 * @param[in] level Transmit power level.
 */
void LoRaDriver::SetTxPower(uint8_t level) {
    if (level < 2) {
        level = 2;
    } else if (level > 17) {
        level = 17;
    }
    this->WriteRegister(Registers::PA_CONFIG,
                        PAConfiguration::PA_BOOST | (level - 2));
}

/**
 * @brief Sets the frequency of the LoRa transceiver.
 *
 * This function sets the frequency by updating the FRF_MSB, FRF_MID, and
 * FRF_LSB registers.
 *
 * @param[in] frequency Desired frequency.
 */
void LoRaDriver::SetFrequency(uint32_t frequency) {
    this->_frequency = frequency;

    uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

    this->WriteRegister(Registers::FRF_MSB, (uint8_t)(frf >> 16));
    this->WriteRegister(Registers::FRF_MID, (uint8_t)(frf >> 8));
    this->WriteRegister(Registers::FRF_LSB, (uint8_t)(frf >> 0));
}

/**
 * @brief Sets the spreading factor of the LoRa transceiver.
 *
 * This function sets the spreading factor by updating the MODEM_CONFIG_2 and
 * MODEM_CONFIG_1 registers, along with configuring detection optimization
 * and detection threshold.
 *
 * @param[in] spreading_factor Desired spreading factor.
 */
void LoRaDriver::SetSpreadingFactor(uint8_t spreading_factor) {
    if (spreading_factor < 6) {
        spreading_factor = 6;
    } else if (spreading_factor > 12) {
        spreading_factor = 12;
    }

    if (spreading_factor == 6) {
        this->WriteRegister(Registers::DETECTION_OPTIMIZE, 0xc5);
        this->WriteRegister(Registers::DETECTION_THRESHOLD, 0x0c);
    } else {
        this->WriteRegister(Registers::DETECTION_OPTIMIZE, 0xc3);
        this->WriteRegister(Registers::DETECTION_THRESHOLD, 0x0a);
    }

    this->WriteRegister(Registers::MODEM_CONFIG_2,
                        (this->ReadRegister(Registers::MODEM_CONFIG_2) & 0x0f) |
                            ((spreading_factor << 4) & 0xf0));
}

/**
 * @brief Sets the bandwidth of the LoRa transceiver.
 *
 * This function sets the bandwidth by updating the MODEM_CONFIG_1 register.
 *
 * @param[in] bandwidth Desired bandwidth.
 */
void LoRaDriver::SetBandwidth(uint32_t bandwidth) {
    uint32_t bw;

    if (bandwidth <= 7.8E3) {
        bw = 0;
    } else if (bandwidth <= 10.4E3) {
        bw = 1;
    } else if (bandwidth <= 15.6E3) {
        bw = 2;
    } else if (bandwidth <= 20.8E3) {
        bw = 3;
    } else if (bandwidth <= 31.25E3) {
        bw = 4;
    } else if (bandwidth <= 41.7E3) {
        bw = 5;
    } else if (bandwidth <= 62.5E3) {
        bw = 6;
    } else if (bandwidth <= 125E3) {
        bw = 7;
    } else if (bandwidth <= 250E3) {
        bw = 8;
    } else {
        bw = 9;
    }
    this->WriteRegister(
        Registers::MODEM_CONFIG_1,
        (this->ReadRegister(Registers::MODEM_CONFIG_1) & 0x0f) | (bw << 4));
}

/**
 * @brief Sets the coding rate denominator of the LoRa transceiver.
 *
 * This function sets the coding rate by updating the MODEM_CONFIG_1 register.
 *
 * @param[in] denominator Desired coding rate denominator.
 */
void LoRaDriver::SetCodingRate(uint8_t denominator) {
    if (denominator < 5) {
        denominator = 5;
    } else if (denominator > 8) {
        denominator = 8;
    }

    int cr = denominator - 4;
    this->WriteRegister(
        Registers::MODEM_CONFIG_1,
        (this->ReadRegister(Registers::MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

/**
 * @brief Sets the preamble length of the LoRa transceiver.
 *
 * This function sets the preamble length by updating the PREAMBLE_MSB and
 * PREAMBLE_LSB registers.
 *
 * @param[in] length Desired preamble length.
 */
void LoRaDriver::SetPreambleLength(uint32_t length) {
    this->WriteRegister(Registers::PREAMBLE_MSB, (uint8_t)(length >> 8));
    this->WriteRegister(Registers::PREAMBLE_LSB, (uint8_t)(length >> 0));
}

/**
 * @brief Sets the sync word of the LoRa transceiver.
 *
 * This function sets the sync word by updating the SYNC_WORD register.
 *
 * @param[in] sync_word Desired sync word.
 */
void LoRaDriver::SetSyncWord(uint8_t sync_word) {
    this->WriteRegister(Registers::SYNC_WORD, sync_word);
}

/**
 * @brief Sets the CRC mode of the LoRa transceiver.
 *
 * This function sets the CRC mode by updating the MODEM_CONFIG_2 register.
 *
 * @param[in] mode CRC mode to set (ENABLE or DISABLE).
 */
void LoRaDriver::SetCRCMode(uint8_t mode) {
    if (mode == CRCMode::ENABLE) {
        this->WriteRegister(
            Registers::MODEM_CONFIG_2,
            this->ReadRegister(Registers::MODEM_CONFIG_2) | 0x04);
    } else {
        this->WriteRegister(
            Registers::MODEM_CONFIG_2,
            this->ReadRegister(Registers::MODEM_CONFIG_2) & 0xfb);
    }
}

/**
 * @brief Sends a packet using the LoRa transceiver.
 *
 * This function sends a packet by setting the transceiver to TX mode and
 * waiting for the transmission to complete.
 *
 * @param[in] pOut Pointer to the data to be transmitted.
 * @param[in] size Size of the data to be transmitted.
 */
void LoRaDriver::SendPacket(uint8_t *pOut, uint8_t size) {
    this->SetIdleMode();
    this->WriteRegister(Registers::FIFO_ADDR_PTR, 0);

    for (uint8_t i = 0; i < size; i++) {
        this->WriteRegister(Registers::FIFO, pOut[i]);
    }

    this->WriteRegister(Registers::PAYLOAD_LENGTH, size);
    this->WriteRegister(Registers::OP_MODE, TransceiverModes::LONG_RANGE_MODE |
                                                TransceiverModes::TX);
    this->_reciever_mode = false;
    uint8_t max_retries = 5;
    uint8_t retries     = 0;
    while (1) {
        uint8_t reg = this->ReadRegister(Registers::IRQ_FLAGS);

        if ((reg & IRQ::TX_DONE_MASK) == IRQ::TX_DONE_MASK)
            break;

        if (retries == max_retries) {
            ESP_LOGE("LoRa Driver", "Error sending the package!");
            break;
        }
        vTaskDelay(2);

        retries++;
    }

    this->WriteRegister(Registers::IRQ_FLAGS, IRQ::TX_DONE_MASK);
    this->SetReceiverMode();
}

/**
 * @brief Receives a packet using the LoRa transceiver.
 *
 * This function receives a packet by checking the RX_DONE_MASK flag and reading
 * the received data if no CRC error occurred.
 *
 * @param[out] pIn Pointer to the buffer where received data will be stored.
 * @param[in] size Size of the buffer for received data.
 * @return The length of the received data.
 */
uint8_t LoRaDriver::ReceivePacket(uint8_t *pIn, uint8_t size) {
    uint8_t received_bytes = 0;

    uint8_t irq = this->ReadRegister(Registers::IRQ_FLAGS);
    this->WriteRegister(Registers::IRQ_FLAGS, irq);

    this->SetIdleMode();
    this->WriteRegister(Registers::FIFO_ADDR_PTR, this->ReadRegister(Registers::FIFO_RX_CURRENT_ADDR));

    do {
        uint8_t length_register = this->_implicit ? Registers::PAYLOAD_LENGTH
                                                  : Registers::RX_NB_BYTES;
        received_bytes          = this->ReadRegister(length_register);
        if (received_bytes > size) {
            received_bytes = 0;
            break;
        }
        for (uint8_t i = 0; i < received_bytes; i++) {
            auto foo = this->ReadRegister(Registers::FIFO);
            pIn[i]   = foo;
        }

    } while (0);

    this->SetReceiverMode();

    return received_bytes;
}

/**
 * @brief Checks if there is data in the LoRa transceiver's receiver buffer.
 *
 * This function checks the RX_DONE_MASK flag to determine if there is data
 * in the transceiver's receiver buffer.
 *
 * @return True if there is data in the receiver buffer, otherwise false.
 */
bool LoRaDriver::isDataInReceiver(void) {
    auto result = false;
    auto irq    = this->ReadRegister(Registers::IRQ_FLAGS);

    do {
        if ((irq & IRQ::RX_DONE_MASK) == 0) {
            break;
        }
        
        if (irq & IRQ::CAD_DETECTED) {
            ESP_LOGD("LoRa Driver", "CAD_DETECTED");
        }
        if (irq & IRQ::FHSS_CHANGE_CHANNEL) {
            ESP_LOGD("LoRa Driver", "FHSS_CHANGE_CHANNEL");
        }
        if (irq & IRQ::CAD_DONE) {
            ESP_LOGD("LoRa Driver", "CAD_DONE");
        }
        if (irq & IRQ::TX_DONE_MASK) {
            ESP_LOGD("LoRa Driver", "TX_DONE_MASK");
        }
        if (irq & IRQ::VALID_HEADER) {
            ESP_LOGD("LoRa Driver", "VALID_HEADER");
        }
        if (irq & IRQ::PAYLOAD_CRC_ERROR_MASK) {
            this->ClearIRQFlag(IRQ::PAYLOAD_CRC_ERROR_MASK);
            ESP_LOGD("LoRa Driver", "PAYLOAD_CRC_ERROR_MASK");
            break;
        }
        if (irq & IRQ::RX_DONE_MASK) {
            ESP_LOGD("LoRa Driver", "RX_DONE_MASK");
        }
        if ((irq & IRQ::RX_TIMEOUT) == IRQ::RX_TIMEOUT) {
            this->ClearIRQFlag(IRQ::RX_TIMEOUT);
            ESP_LOGD("LoRa Driver", "RX_TIMEOUT");
            break;
        }

        result = true;
    } while (0);

    return result;
}

/**
 * @brief Gets the RSSI of the last received packet.
 *
 * This function calculates and returns the RSSI of the last received packet.
 *
 * @return The RSSI value.
 */
uint32_t LoRaDriver::GetLastPacketRSSI(void) {
    return (this->ReadRegister(Registers::PKT_RSSI_VALUE) -
            (this->_frequency < 868E6 ? 164 : 157));
}

/**
 * @brief Gets the SNR of the last received packet.
 *
 * This function gets and returns the Signal-to-Noise Ratio (SNR) of the last
 * received packet.
 *
 * @return The SNR value.
 */
uint8_t LoRaDriver::GetLastPacket4TSNR(void) {
    return ((int8_t)this->ReadRegister(Registers::PKT_SNR_VALUE));
}

/**
 * @brief Writes a value to a LoRa register.
 *
 * This function writes a value to the specified LoRa register.
 *
 * @param[in] register_address Address of the register to write to.
 * @param[in] register_value Value to write to the register.
 * @return ESP_OK if the write operation is successful, otherwise an error code.
 */
titan_err_t LoRaDriver::WriteRegister(uint8_t register_address,
                                      uint8_t register_value) {
    uint8_t write_address = Driver::WRITE_COMMAND | register_address;
    uint8_t out[2]        = {write_address, register_value};
    uint8_t in[2];

    return spi_manager->DeviceTransmit(out, in, sizeof(out));
}

/**
 * @brief Reads a value from a LoRa register.
 *
 * This function reads a value from the specified LoRa register.
 *
 * @param[in] register_address Address of the register to read from.
 * @return The value read from the register.
 */
uint8_t LoRaDriver::ReadRegister(uint8_t register_address) {
    uint8_t out[2] = {register_address, 0xff};
    uint8_t in[2]  = {0};

    spi_manager->DeviceTransmit(out, in, sizeof(out));

    return in[1];
}

/**
 * @brief Validates the version of the LoRa transceiver.
 *
 * This function validates the version of the LoRa transceiver by comparing it
 * with the expected version. Retries the validation up to five times.
 *
 * @return ESP_OK if the version is valid, otherwise an error code.
 */
titan_err_t LoRaDriver::ValidateVersion(void) {
    auto result      = Error::UNKNOW_FAIL;
    auto max_retires = 5;

    for (uint8_t i = 0; i < max_retires; i++) {
        uint8_t read_value = this->ReadRegister(Registers::VERSION);

        if (read_value == Driver::VERSION) {
            result = Error::NO_ERROR;
            break;
        }
    }

    return result;
}

/**
 * @brief Clears the specified IRQ (Interrupt Request) flag(s) in the LoRa module.
 *
 * This function reads the current IRQ flags from the specified register, 
 * clears the bits specified by the `irq_flag_mask`, and writes the updated value 
 * back to the IRQ flags register.
 *
 * @param irq_flag_mask A bitmask specifying which IRQ flags to clear. 
 *                      Each bit in the mask corresponds to an IRQ flag in the LoRa module.
 */
void LoRaDriver::ClearIRQFlag(uint8_t irq_flag_mask) {
        uint8_t irq = this->ReadRegister(Registers::IRQ_FLAGS);
        irq &= ~irq_flag_mask;

        this->WriteRegister(Registers::IRQ_FLAGS, irq);
}