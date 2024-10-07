#include "GPIOManager.h"

/**
 * @brief Initializes the GPIOManager.
 *
 * This function initializes the GPIOManager by setting up the GPIO pins
 * according to the predefined configuration in 'gpio_internal_list'.
 *
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
titan_err_t GPIOManager::Initialize(void) {
    auto result = ESP_OK;

    this->_gpio_array_list_size = BoardConfig::CONFIGURED_PINS;

    for (uint8_t i = 0; i < this->_gpio_array_list_size; i++) {
        this->_gpio_internal_list[i] = new GPIOInternal(BoardConfig::PINS_NAME[i], BoardConfig::PINS_MODE[i]);
    }

    return result;
}

/**
 * @brief Writes the state to the specified GPIO pin.
 *
 * This function writes the specified state to the GPIO pin identified by `id`.
 *
 * @param[in] id The ID of the GPIO pin.
 * @param[in] state The state to be written (HIGH or LOW).
 * @return ESP_OK if writing is successful, otherwise an error code.
 */
titan_err_t GPIOManager::WriteGPIO(gpio_id_et id, state_gpio_et state) {
    auto result                 = Error::UNKNOW_FAIL;
    GPIOInternal *selected_gpio = this->GetGPIO(id);

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
 * @param[in] id The ID of the GPIO pin.
 * @return The state of the GPIO pin (HIGH or LOW), or -1 if an error occurs.
 */
uint8_t GPIOManager::ReadGPIO(gpio_id_et id) {
    auto result        = Error::UNKNOW_FAIL;
    auto selected_gpio = this->GetGPIO(id);

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
 * @param[in] id The ID of the GPIO pin.
 * @return A pointer to the internal GPIO structure, or nullptr if not found.
 */
GPIOInternal *GPIOManager::GetGPIO(gpio_id_et id) {
    for (uint8_t i = 0; i < this->_gpio_array_list_size; i++) {
        if (this->_gpio_internal_list[i]->id() == id) {
            return this->_gpio_internal_list[i];
        }
    }

    return nullptr;
}
