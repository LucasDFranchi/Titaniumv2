#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include "GPIOInternal.h"
#include "Kernel/error/error_enum.h"
#include "driver/gpio.h"

/**
 * @brief Manages GPIO interfaces.
 */
class GPIOManager {
   public:
    GPIOManager(const GPIOManager& obj) = delete;
    static GPIOManager* GetInstance(void);

   public:
    titan_err_t Initialize(void);
    titan_err_t WriteGPIO(gpio_id_et id, state_gpio_et state);
    uint8_t ReadGPIO(gpio_id_et id);

   private:
    GPIOManager() {}
    static GPIOManager* singleton_pointer_;

   private:
    GPIOInternal* GetGPIO(gpio_id_et id);

   private:
    uint8_t _gpio_array_list_size;
};

#endif /* GPIO_MANAGER_H */
