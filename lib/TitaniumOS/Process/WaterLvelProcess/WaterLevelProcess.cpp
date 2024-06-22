#include "WaterLevelProcess.h"
#include "Managers/ManagersAreaIndex.h"

#include "esp_timer.h"
#include "stdlib.h"

void WaterLevelProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    this->_gpio_manager = GPIOManager::GetInstance();

    while (1) {
        //this->_shared_memory_manager.get()->Write(ManagersAreaIndex::UART, sizeof(water_level_st), const_cast<char*>(arr[cont]));
        this->UpdateWaterLevel(this->GetWaterLevel());

        //to Debug only
        //printf("distance %s\n", (char*)this->_water_level);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * Updates the internal WaterLevel variable 
 *
 * .
 */
void WaterLevelProcess::UpdateWaterLevel(uint32_t distance)
{
    utoa(distance, (char*)this->_water_level, 10);
    this->_water_level[3] = '/0';
}

/**
 * Runs the whole process to get the water level from sensor (jsn-sr04t), mode 0.
 *
 * @returns The distance in uint32_t .
 */
uint32_t WaterLevelProcess::GetWaterLevel()
{   
    //printf("init reading distance\n");
    this->_gpio_manager->WriteGPIO(SENSOR_TRIG, state_gpio_et::HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    this->_gpio_manager->WriteGPIO(SENSOR_TRIG, state_gpio_et::LOW);

    auto echo_level = this->_gpio_manager->ReadGPIO(SENSOR_ECHO);
    while (echo_level == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        echo_level = this->_gpio_manager->ReadGPIO(SENSOR_ECHO);
        if(this->IncrementAndCheckTimeout())
        {
            this->ResetTimeout();
            //printf("Error, sending trigger again\n");
            return 0;
        }
    } ;

    this->ResetTimeout();

    int64_t start_time = esp_timer_get_time();
    do
    {
      echo_level = this->_gpio_manager->ReadGPIO(SENSOR_ECHO);
      if(this->IncrementAndCheckTimeout())
      {
          this->ResetTimeout();
          //printf("Error, sending trigger again\n");
          return 0;
      }
    } while (echo_level);

    int64_t elapsed_time = esp_timer_get_time() - start_time;
    
    return this->CalculateDistance(elapsed_time);
    //printf("Distance: %ld, count %lld \n", this->_distance, elapsed_time);
}
/**
 * Reset internal timeout for the water level
 *
 * 
 */
void WaterLevelProcess::ResetTimeout()
{
    this->_timeout_count = 0;
}

/**
 * Increment timer and check if timeout has beeing reached, 
 *
 * @return if timeout has reached WATER_LEVEL_TIMEOUT
 */
uint8_t WaterLevelProcess::IncrementAndCheckTimeout()
{
    this->_timeout_count++;
    
    return this->_timeout_count > WATER_LEVEL_TIMEOUT;
}

/**
 * Calculates the distance as per the sensor echo reading.
 *
 * @returns The distance in uint32_t .
 */
uint32_t WaterLevelProcess::CalculateDistance(uint32_t timer_count)
{
    return timer_count/58;
}

/**
 * Initializes the SerialCommunicationManager.
 *
 * @returns The result of the initialization process.
 */
esp_err_t WaterLevelProcess::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());
    return ESP_OK;
}