#include "SPIManager.h"
#include "HAL/inc/BoardHeader.hpp"

/**
 * @brief Initializes the SPIManager and configures the SPI bus and device.
 *
 * This function initializes the SPIManager by configuring the SPI bus and
 * device parameters.
 *
 * @param[in] mosi GPIO number for MOSI line.
 * @param[in] miso GPIO number for MISO line.
 * @param[in] sclk GPIO number for SCLK line.
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
titan_err_t SPIManager::Initialize() {
    auto result = ESP_OK;

    this->_bus.mosi_io_num = BoardConfig::SPI_MOSI;
    this->_bus.miso_io_num = BoardConfig::SPI_MISO;
    this->_bus.sclk_io_num = BoardConfig::SPI_SCK;
    this->_bus.quadwp_io_num = -1;
    this->_bus.quadhd_io_num = -1;
    this->_bus.max_transfer_sz = 1024;

    result = spi_bus_initialize(BoardConfig::SPI_HOST_DEVICE, &this->_bus, 0);

    this->_dev.mode = 0;
    this->_dev.clock_speed_hz = (SPI_MASTER_FREQ_80M / 200),
    this->_dev.spics_io_num = SPI_CS;
    this->_dev.flags = 0;
    this->_dev.queue_size = 1;
    this->_dev.pre_cb = nullptr;
    result += spi_bus_add_device(BoardConfig::SPI_HOST_DEVICE, &this->_dev, &this->_spi_device);
    
    return result;
}

/**
 * @brief Performs a SPI transaction to transmit and receive data.
 *
 * This function performs a SPI transaction to transmit data and receive the
 * response.
 *
 * @param[in] transmission_packet Pointer to the data to be transmitted.
 * @param[out] receive_packet Pointer to the buffer for received data.
 * @param[in] size Size of the data to be transmitted and received.
 * @return ESP_OK if the transaction is successful, otherwise an error code.
 */
titan_err_t SPIManager::DeviceTransmit(uint8_t* transmission_packet,
                                     uint8_t* receive_packet, uint8_t size) {
    auto result = Error::UNKNOW_FAIL;

    spi_transaction_t transaction_command{};
    transaction_command.flags = 0;
    transaction_command.length = 8 * size,
    transaction_command.tx_buffer = transmission_packet;
    transaction_command.rx_buffer = receive_packet;

    result = spi_device_transmit(this->_spi_device, &transaction_command);
    ESP_ERROR_CHECK(result);
    return result;
}

/**
 * @brief Returns the singleton instance of SPIManager.
 *
 * @return A pointer to the singleton instance of SPIManager.
 */
SPIManager* SPIManager::GetInstance(void) {
    if (singleton_pointer_ == nullptr) {
        singleton_pointer_ = new SPIManager();
    }

    return singleton_pointer_;
}
