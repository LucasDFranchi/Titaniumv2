#ifndef GPIO_INTERNAL_GUARD
#define GPIO_INTERNAL_GUARD

#include "GPIOEnums.h"

#include "stdint.h"

#include "esp_err.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"


/**
 * Task safe GPIO class
 */
class GPIOInternal {
public:
    /**
     * @brief Constructor for GPIOInternal class.
     * 
     * @param id The GPIO ID to initialize.
     * @param mode The mode of the GPIO pin.
     */
    GPIOInternal(gpio_id_et id, gpio_mode_t mode) : _id(id), _mode(mode) {
        auto result = ESP_OK;
        this->_mutex = xSemaphoreCreateMutex();

        result += gpio_reset_pin(static_cast<gpio_num_t>(this->_id));
        result += gpio_set_direction(static_cast<gpio_num_t>(this->_id), this->_mode);

        this->_is_initialized = result == ESP_OK ? true : false;
    }

    /**
     * @brief Get the ID of the GPIO pin.
     * 
     * @return The ID of the GPIO pin.
     */
    gpio_id_et id() const {
        return this->_id;
    }

    /**
     * @brief Get the mode of the GPIO pin.
     * 
     * @return The mode of the GPIO pin.
     */
    gpio_mode_t mode() const {
        return this->_mode;
    }

    /**
     * @brief Check if the GPIO pin is initialized.
     * 
     * @return True if the GPIO pin is initialized, false otherwise.
     */
    bool is_initialized() const {
        return this->_is_initialized;
    }

    /**
     * @brief Set the state of the GPIO pin.
     * 
     * This function sets the state of the GPIO pin to the specified state.
     * 
     * @param state The state to set the GPIO pin to (LOW or HIGH).
     * @return ESP_OK if the operation was successful, an error code otherwise.
     */
    esp_err_t SetGPIOState(state_gpio_et state) {
        auto result = ESP_OK;

        if (xSemaphoreTake(this->_mutex, portMAX_DELAY)) {
            gpio_set_level(static_cast<gpio_num_t>(this->_id), state);
            result = ESP_OK;
            xSemaphoreGive(this->_mutex);
        }

        return result;
    }

    /**
     * @brief Get the state of the GPIO pin.
     * 
     * This function retrieves the current state of the GPIO pin.
     * 
     * @return The state of the GPIO pin (LOW or HIGH).
     */
    state_gpio_et GetGPIOState(void) {
        int result = UNDEFINED;

        if (xSemaphoreTake(this->_mutex, portMAX_DELAY)) {
            result = gpio_get_level(static_cast<gpio_num_t>(this->_id));
            xSemaphoreGive(this->_mutex);
        }

        return static_cast<state_gpio_et>(result);
    }

private:
    gpio_id_et _id;
    gpio_mode_t _mode;
    bool _is_initialized;
    SemaphoreHandle_t _mutex;
};


#endif /* GPIO_INTERNAL_GUARD */