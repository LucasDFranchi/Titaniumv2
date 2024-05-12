#ifndef SPI_MANAGER_GUARD
#define SPI_MANAGER_GUARD

#include "stdint.h"

#include "HAL/gpio/GPIOManager.h"

#include "esp_err.h"
#include "driver/spi_master.h"

/**
 * @brief Manages SPI device
 */
class SPIManager{
    public:
    SPIManager(const SPIManager& obj) = delete;
    static SPIManager* GetInstance(void);

    public:
    esp_err_t Initialize(void);
    esp_err_t DeviceTransmit(uint8_t* transmission_packet, uint8_t* receive_packet, uint8_t size);
    
    private:
    SPIManager(){};
    static SPIManager* singleton_pointer_;

    private:
    spi_device_handle_t           _spi_device{};
    spi_bus_config_t              _bus{};
    spi_device_interface_config_t _dev{};
    GPIOManager*                  _gpio_manager;
};

#endif /* SPI_MANAGER_GUARD */