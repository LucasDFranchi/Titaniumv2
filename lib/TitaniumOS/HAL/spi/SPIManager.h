#ifndef SPI_MANAGER_H
#define SPI_MANAGER_H

#include "stdint.h"

#include "HAL/gpio/GPIOManager.h"

#include "Application/error/error_enum.h"
#include "driver/spi_master.h"

/**
 * @brief Manages SPI device.
 */
class SPIManager {
   public:
    SPIManager(const SPIManager& obj) = delete;
    static SPIManager* GetInstance(void);

   public:
    titan_err_t Initialize(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk);
    titan_err_t DeviceTransmit(uint8_t* transmission_packet, uint8_t* receive_packet, uint8_t size);

   private:
    SPIManager() {};
    static SPIManager* singleton_pointer_;

   private:
    spi_device_handle_t _spi_device{};    /**< SPI device handle. */
    spi_bus_config_t _bus{};              /**< SPI bus configuration. */
    spi_device_interface_config_t _dev{}; /**< SPI device configuration. */
    GPIOManager* _gpio_manager;           /**< Pointer to GPIOManager instance. */
};

#endif /* SPI_MANAGER_H */
