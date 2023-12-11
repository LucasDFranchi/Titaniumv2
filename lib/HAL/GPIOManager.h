#include "stdint.h"

#include "esp_err.h"
#include "driver/gpio.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"

#ifndef GPIO_MANAGER_GUARD
#define GPIO_MANAGER_GUARD

typedef enum gpio_id_e{
    LED_WHITE = GPIO_NUM_25,
    SPI_CS    = GPIO_NUM_18,
    LORA_RST  = GPIO_NUM_14,
} gpio_id_et;

typedef enum state_gpio_et {
    LOW = 0,
    HIGH = 1
}state_gpio_et;

typedef struct gpio_internal_s
{
    gpio_id_et id;
    bool is_initialized;
    SemaphoreHandle_t mutex;
    gpio_mode_t mode;
} gpio_internal_st;

/**
 * Manages io interface.
 */
class GPIOManager{
    public:
    GPIOManager(const GPIOManager& obj) = delete;
    esp_err_t Initialize(void);
    esp_err_t WriteGPIO(gpio_id_et id, state_gpio_et state);
    uint8_t   ReadGPIO(gpio_id_et id);
    static GPIOManager* GetInstance(void);
    
    private:
    GPIOManager(){};
    gpio_internal_st* GetGPIO_(gpio_id_et id);
    uint8_t gpio_array_list_size_;
    static GPIOManager* singleton_pointer_;
};

#endif /* GPIO_MANAGER_GUARD */