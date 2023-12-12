#include "GPIOManager.h"

gpio_internal_st gpio_internal_list[] = {
    {
        .id = GPIO_NUM_25,
        .is_initialized = false,
        .mutex = nullptr,
        .mode = GPIO_MODE_INPUT_OUTPUT,
    }};

esp_err_t GPIOManager::Initialize(void)
{
    auto result = ESP_OK;
    this->gpio_array_list_size_ = sizeof(gpio_internal_list) / sizeof(gpio_internal_list[0]);

    for (uint8_t i = 0; i < this->gpio_array_list_size_; i++)
    {
        gpio_internal_list[i].is_initialized = true;
        gpio_internal_list[i].mutex = xSemaphoreCreateMutex();
        gpio_reset_pin(gpio_internal_list[i].id);
        gpio_set_direction(gpio_internal_list[i].id, gpio_internal_list[i].mode);
        gpio_set_level(GPIO_NUM_25, LOW);
    }

    return result;
}

esp_err_t GPIOManager::WriteGPIO(gpio_num_t id, state_gpio_et state)
{
    auto result = ESP_FAIL;
    auto selected_gpio = this->GetGPIO_(id);

    do
    {
        if (selected_gpio == nullptr)
        {
            break;
        }
        if (selected_gpio->mutex == nullptr)
        {
            break;
        }

        if (xSemaphoreTake(selected_gpio->mutex, portMAX_DELAY))
        {
            gpio_set_level(id, state);
            result = ESP_OK;
            xSemaphoreGive(selected_gpio->mutex);
        }

    } while (0);

    return result;
}

uint8_t GPIOManager::ReadGPIO(gpio_num_t id)
{
    auto result = -1;
    auto selected_gpio = this->GetGPIO_(id);

    do
    {
        if (selected_gpio == nullptr)
        {
            break;
        }
        if (selected_gpio->mutex == nullptr)
        {
            break;
        }
        if (xSemaphoreTake(selected_gpio->mutex, portMAX_DELAY))
        {
            result = gpio_get_level(id);
            xSemaphoreGive(selected_gpio->mutex);
        }
    } while (0);

    return result;
}

gpio_internal_st *GPIOManager::GetGPIO_(gpio_num_t id)
{
    for (uint8_t i = 0; i < this->gpio_array_list_size_; i++)
    {
        if (gpio_internal_list[i].id == id)
        {
            return &gpio_internal_list[i];
        }
    }

    return nullptr;
}

/**
 * @brief Returns the singleton instance
 * 
 * @return GPIOManager* 
 */
GPIOManager* GPIOManager::GetInstance(void)
  {
    if (singleton_pointer_ == nullptr)
    {
      singleton_pointer_ = new GPIOManager();
    }

    return singleton_pointer_;
  }
