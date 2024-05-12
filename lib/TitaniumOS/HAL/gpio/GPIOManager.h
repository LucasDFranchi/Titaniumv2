#ifndef GPIO_MANAGER_GUARD
#define GPIO_MANAGER_GUARD

#include "stdint.h"

#include "esp_err.h"
#include "driver/gpio.h"

#include "GPIOInternal.h"


/**
 * @brief Manages io interface.
 */
class GPIOManager{

    public:
    GPIOManager(const GPIOManager& obj) = delete;
    static GPIOManager* GetInstance(void);
    
    public:
    esp_err_t Initialize(void);
    esp_err_t WriteGPIO(gpio_id_et id, state_gpio_et state);
    uint8_t   ReadGPIO(gpio_id_et id);

    private:
    GPIOManager(){};
    static GPIOManager* singleton_pointer_;
    
    private:
    GPIOInternal* _GetGPIO(gpio_id_et id);
    uint8_t _gpio_array_list_size;
};

#endif /* GPIO_MANAGER_GUARD */