/* This file is autogenerated. Do not edit manually. It is generated*/
/* before the build process based on the board configuration */
#pragma once

#pragma once

#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/i2c.h"

typedef enum gpio_id_t{
	LORA_RST = GPIO_NUM_14,
	LED_WHITE = GPIO_NUM_25,
	SPI_CS = GPIO_NUM_5,
} gpio_id_et;

namespace BoardConfig {

    // GPIO Pins
    static constexpr uint8_t CONFIGURED_PINS = 3;
    static constexpr gpio_id_et PINS_NAME[] = {
			LORA_RST,
			LED_WHITE,
			SPI_CS
		};

    static constexpr gpio_mode_t PINS_MODE[] = {
			GPIO_MODE_OUTPUT,
 			GPIO_MODE_OUTPUT,
 			GPIO_MODE_OUTPUT
		};

    // SPI Pins
    static constexpr bool CONFIGURED_SPI = true;
    static constexpr spi_host_device_t SPI_HOST_DEVICE = SPI2_HOST;
    static constexpr gpio_num_t SPI_MOSI = GPIO_NUM_23;
    static constexpr gpio_num_t SPI_MISO = GPIO_NUM_19;
    static constexpr gpio_num_t SPI_SCK = GPIO_NUM_18;
    // I2C Pins

    static constexpr bool CONFIGURED_I2C = true;
    static constexpr gpio_num_t I2C_SDA = GPIO_NUM_21;
    static constexpr gpio_num_t I2C_SCL = GPIO_NUM_22
;
};  // namespace BoardConfig
