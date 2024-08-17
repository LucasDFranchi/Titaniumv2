/**
 * @file HTTPServerProcess.cpp
 * @brief Implementation of HTTPServerProcess class methods and HTTP request handlers.
 */

#include "CustomProcess/TimeProcess/inc/TimeProcess.h"
#include "HAL/memory/MemoryHandlers.h"

#include "esp_log.h"
#include "esp_timer.h"

/**
 * @brief Executes the time processing tasks.
 *
 * This method contains the main logic for time processing. It is
 * responsible for handling periodic tasks related to time management.
 */
void TimeProcess::Execute(void) {
    this->Initialize();
    uint8_t mode = 0;

    while (1) {
        if (mode == 0) {
            this->_shared_memory_manager->Read(MEMORY_AREAS_TIME_PROCESS, this->time_process, time_process_t_msg);
        } else {
            uint64_t elapsed_time_us = esp_timer_get_time();

            // Convert to seconds for easier reading
            uint32_t elapsed_time_sec = elapsed_time_us / 1000000;
            // Create a simple timer format
            uint32_t hours   = elapsed_time_sec / 3600;
            uint32_t minutes = (elapsed_time_sec % 3600) / 60;
            uint32_t seconds = elapsed_time_sec % 60;

            this->time_process.raw_time = elapsed_time_us;
            this->time_process.hours    = hours;
            this->time_process.minutes  = minutes;
            this->time_process.seconds  = seconds;

            this->_shared_memory_manager->Write(MEMORY_AREAS_TIME_PROCESS, this->time_process, time_process_t_msg);
        }

        ESP_LOGI("Timer", "Elapsed time since boot: %02lu:%02lu:%02lu", this->time_process.hours,
                 this->time_process.minutes,
                 this->time_process.seconds);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Initializes the time processing module.
 *
 * This method performs necessary setup for the time processing tasks,
 * including configuring resources and preparing the environment.
 *
 * @return titan_err_t Error code indicating success or failure of
 * initialization.
 */
titan_err_t TimeProcess::Initialize(void) {
    auto result = ESP_OK;

    this->_shared_memory_manager = SharedMemoryManager::GetInstance();

    return result;
}
