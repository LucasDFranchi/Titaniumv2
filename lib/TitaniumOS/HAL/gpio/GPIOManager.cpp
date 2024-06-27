#include "GPIOManager.h"

// TODO: Find a better place for this struct, should be in a config file.
/**
 * @brief Internal structure to store GPIO configuration.
 */
GPIOInternal gpio_internal_list[] = {
    GPIOInternal(LED_WHITE, GPIO_MODE_OUTPUT),
    GPIOInternal(SPI_CS, GPIO_MODE_OUTPUT),
    GPIOInternal(LORA_RST, GPIO_MODE_OUTPUT),
};

/**
 * @brief Initializes the GPIOManager.
 *
 * This function initializes the GPIOManager by setting up the GPIO pins
 * according to the predefined configuration in 'gpio_internal_list'.
 *
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t GPIOManager::Initialize(void) {
    auto result = ESP_OK;

    this->_gpio_array_list_size =
        sizeof(gpio_internal_list) / sizeof(gpio_internal_list[0]);

    return result;
}

/**
 * @brief Writes the state to the specified GPIO pin.
 *
 * This function writes the specified state to the GPIO pin identified by `id`.
 *
 * @param id The ID of the GPIO pin.
 * @param state The state to be written (HIGH or LOW).
 * @return ESP_OK if writing is successful, otherwise an error code.
 */
esp_err_t GPIOManager::WriteGPIO(gpio_id_et id, state_gpio_et state) {
    auto result = ESP_FAIL;
    GPIOInternal *selected_gpio = this->_GetGPIO(id);

    do {
        if (selected_gpio == nullptr) {
            break;
        }

        if (selected_gpio->is_initialized() == false) {
            break;
        }

        result = selected_gpio->SetGPIOState(state);

    } while (0);

    return result;
}

/**
 * @brief Reads the state from the specified GPIO pin.
 *
 * This function reads and returns the state of the GPIO pin identified by `id`.
 *
 * @param id The ID of the GPIO pin.
 * @return The state of the GPIO pin (HIGH or LOW), or -1 if an error occurs.
 */
uint8_t GPIOManager::ReadGPIO(gpio_id_et id) {
    auto result = -1;
    auto selected_gpio = this->_GetGPIO(id);

    do {
        if (selected_gpio == nullptr) {
            break;
        }

        if (selected_gpio->is_initialized() == false) {
            break;
        }

        result = selected_gpio->GetGPIOState();

    } while (0);

    return result;
}

/**
 * @brief Returns the singleton instance of GPIOManager.
 *
 * @return A pointer to the singleton instance of GPIOManager.
 */
GPIOManager *GPIOManager::GetInstance(void) {
    if (singleton_pointer_ == nullptr) {
        singleton_pointer_ = new GPIOManager();
    }

    return singleton_pointer_;
}

/**
 * @brief Gets the internal GPIO structure for the specified GPIO ID.
 *
 * This function retrieves the internal GPIO structure for the specified GPIO
 * ID.
 *
 * @param id The ID of the GPIO pin.
 * @return A pointer to the internal GPIO structure, or nullptr if not found.
 */
GPIOInternal* GPIOManager::_GetGPIO(gpio_id_et id) {
    for (uint8_t i = 0; i < this->_gpio_array_list_size; i++) {
        if (gpio_internal_list[i].id() == id) {
            return &gpio_internal_list[i];
        }
    }

    return nullptr;
}
