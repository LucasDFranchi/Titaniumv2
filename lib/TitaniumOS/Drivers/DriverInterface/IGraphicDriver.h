#ifndef GRAPHIC_DRIVER_INTERFACE_H
#define GRAPHIC_DRIVER_INTERFACE_H

#include "esp_err.h"

/**
 * @brief Interface for a graphic driver.
 *
 * This interface defines the methods required for a graphic driver.
 */
class IGraphicDriver {
    public:
    /**
     * @brief Initialize the graphic driver.
     *
     * @return esp_err_t ESP_OK on success, or an error code on failure.
     */
    virtual esp_err_t Initialize(void) = 0;

    /**
     * @brief Process the graphic data.
     *
     * @param memory_area_data Pointer to the memory area data to be processed.
     */
    virtual void Process(uint8_t *memory_area_data) = 0;
};

#endif /* GRAPHIC_DRIVER_INTERFACE_H */
