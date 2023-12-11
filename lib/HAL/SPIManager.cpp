#include "SPIManager.h"

#include "GPIOManager.h"

esp_err_t SPIManager::Initialize(void){

    auto result = ESP_OK;
    this->gpio_manager = GPIOManager::GetInstance();

    this->_bus.mosi_io_num = GPIO_NUM_27;
    this->_bus.miso_io_num = GPIO_NUM_19;
    this->_bus.sclk_io_num = GPIO_NUM_5;
    this->_bus.quadwp_io_num = -1;
    this->_bus.quadhd_io_num = -1;
    this->_bus.max_transfer_sz = 1024;

    result = spi_bus_initialize(SPI2_HOST , &this->_bus, 0);
    ESP_ERROR_CHECK(result);

    this->_dev.mode = 0;
    this->_dev.clock_speed_hz = SPI_MASTER_FREQ_9M,
    this->_dev.spics_io_num = -1;
    this->_dev.flags = 0;
    this->_dev.queue_size = 1;
    this->_dev.pre_cb = nullptr;
    result += spi_bus_add_device(SPI2_HOST , &this->_dev, &this->_spi_device);
    ESP_ERROR_CHECK(result);

    return result;
}

esp_err_t SPIManager::DeviceTransmit(uint8_t* transmission_packet, uint8_t* receive_packet, uint8_t size){
    auto result = ESP_FAIL;
    
    spi_transaction_t transaction_command{};
    transaction_command.flags = 0;
    transaction_command.length = 8 * size,
    transaction_command.tx_buffer = transmission_packet;
    transaction_command.rx_buffer = receive_packet;

    this->gpio_manager->WriteGPIO(SPI_CS, LOW);
    result = spi_device_transmit(this->_spi_device, &transaction_command);
    this->gpio_manager->WriteGPIO(SPI_CS, HIGH);
    ESP_ERROR_CHECK(result);
    return result;
}

/**
 * @brief Returns the singleton instance
 *
 * @return SPIManager*
 */
SPIManager *SPIManager::GetInstance(void)
{
    if (singleton_pointer_ == nullptr)
    {
        singleton_pointer_ = new SPIManager();
    }

    return singleton_pointer_;
}