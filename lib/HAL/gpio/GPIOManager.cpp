#include "gpio/GPIOManager.h"

/**
 * @brief Internal structure to store GPIO configuration.
 */
gpio_internal_st gpio_internal_list[] = {
    {
        .id = LED_WHITE,
        .is_initialized = false,
        .mutex = nullptr,
        .mode = GPIO_MODE_INPUT_OUTPUT,
    },
    {
        .id = SPI_CS,
        .is_initialized = false,
        .mutex = nullptr,
        .mode = GPIO_MODE_OUTPUT,
    },
    {
        .id = LORA_RST,
        .is_initialized = false,
        .mutex = nullptr,
        .mode = GPIO_MODE_OUTPUT,
    },
};

/**
 * @brief Initializes the GPIOManager.
 *
 * This function initializes the GPIOManager by setting up the GPIO pins
 * according to the predefined configuration in `gpio_internal_list`.
 *
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t GPIOManager::Initialize(void) {
    auto result = ESP_OK;
    this->gpio_array_list_size_ =
        sizeof(gpio_internal_list) / sizeof(gpio_internal_list[0]);

    for (uint8_t i = 0; i < 2; i++) {
        gpio_internal_list[i].is_initialized = true;
        gpio_internal_list[i].mutex = xSemaphoreCreateMutex();
        gpio_reset_pin((gpio_num_t)gpio_internal_list[i].id);
        gpio_set_direction((gpio_num_t)gpio_internal_list[i].id,
                           gpio_internal_list[i].mode);
    }

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
    auto selected_gpio = this->GetGPIO_(id);

    do {
        if (selected_gpio == nullptr) {
            break;
        }
        if (selected_gpio->mutex == nullptr) {
            break;
        }

        if (xSemaphoreTake(selected_gpio->mutex, portMAX_DELAY)) {
            gpio_set_level((gpio_num_t)id, state);
            result = ESP_OK;
            xSemaphoreGive(selected_gpio->mutex);
        }

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
    auto selected_gpio = this->GetGPIO_(id);

    do {
        if (selected_gpio == nullptr) {
            break;
        }
        if (selected_gpio->mutex == nullptr) {
            break;
        }
        if (xSemaphoreTake(selected_gpio->mutex, portMAX_DELAY)) {
            result = gpio_get_level((gpio_num_t)id);
            xSemaphoreGive(selected_gpio->mutex);
        }
    } while (0);

    return result;
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
gpio_internal_st *GPIOManager::GetGPIO_(gpio_id_et id) {
    for (uint8_t i = 0; i < this->gpio_array_list_size_; i++) {
        if (gpio_internal_list[i].id == id) {
            return &gpio_internal_list[i];
        }
    }

    return nullptr;
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
