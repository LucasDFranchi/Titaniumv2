#ifndef GPIO_ENUMERATIONS_GUARD
#define GPIO_ENUMERATIONS_GUARD

#include "driver/gpio.h"

/**
 * @brief Enumeration of GPIO IDs.
 */
typedef enum gpio_id_e {
    LED_WHITE = GPIO_NUM_25, /**< GPIO ID for white LED. */
    SPI_CS    = GPIO_NUM_18, /**< GPIO ID for SPI chip select. */
    LORA_RST  = GPIO_NUM_14  /**< GPIO ID for LoRa reset. */
} gpio_id_et;

/**
 * @brief Enumeration of GPIO states.
 */
typedef enum state_gpio_et {
    LOW       = 0, /**< Low state for GPIO. */
    HIGH      = 1, /**< High state for GPIO. */
    UNDEFINED = -1 /**< Undefined state for GPIO. */
} state_gpio_et;

#endif /* GPIO_ENUMERATIONS_GUARD */
