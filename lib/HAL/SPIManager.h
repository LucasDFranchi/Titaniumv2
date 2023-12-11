#include "stdint.h"

#include "GPIOManager.h"

#include "esp_err.h"
#include "driver/spi_master.h"


#ifndef SPI_MANAGER_GUARD
#define SPI_MANAGER_GUARD

/**
 * Manages spi interface.
 */
class SPIManager{
    public:
    SPIManager(const SPIManager& obj) = delete;
    esp_err_t Initialize(void);
    esp_err_t DeviceTransmit(uint8_t* transmission_packet, uint8_t* receive_packet, uint8_t size);
    static SPIManager* GetInstance(void);
    
    private:
    SPIManager(){};
    static SPIManager* singleton_pointer_;

    spi_device_handle_t           _spi_device{};
    spi_bus_config_t              _bus{};
    spi_device_interface_config_t _dev{};
    GPIOManager* gpio_manager = nullptr;
};

#endif /* SPI_MANAGER_GUARD */