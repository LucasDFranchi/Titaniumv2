#include "nvs_flash.h"

#include <HAL/memory/SharedMemoryManager.h>
#include <HAL/gpio/GPIOManager.h>
#include <HAL/spi/SPIManager.h>
// #include <gpio/GPIOManager.h>
// #include <spi/SPIManager.h>

#include <Managers/SerialCommunicationManager/src/SerialCommunicationManager.h>
#include <Managers/NetworkManager/src/NetworkManager.h>
// #include <CommandManager.h>
// #include <GraphicDriverManager.h>
// #include <LoRaManager.h>
// #include <HTTPServerManager.h>

#include <Process/WaterLvelProcess/WaterLevelProcess.h>

SharedMemoryManager* SharedMemoryManager::singleton_pointer_ = nullptr;
GPIOManager* GPIOManager::singleton_pointer_                 = nullptr;
SPIManager* SPIManager::singleton_pointer_                   = nullptr;

esp_err_t initialize_nvs(void) {
    nvs_flash_erase();
    esp_err_t ret = nvs_flash_init();
    return ret;
}

int main(void) {
    initialize_nvs();

    auto shared_memory_manager = SharedMemoryManager::GetInstance();
    auto gpio_manager          = GPIOManager::GetInstance();
    auto spi_manager           = SPIManager::GetInstance();

    shared_memory_manager->Initialize();
    gpio_manager->Initialize();
    spi_manager->Initialize();

    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::SCRATCH, 1024, READ_WRITE));
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::CREDENTIALS, sizeof(credentials_st), READ_WRITE));
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::CONNECTION, sizeof(connection_st), READ_WRITE));
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::UART, 16, READ_WRITE));
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::LORA, 1024, READ_WRITE));

    auto uart_communication_manager = new SerialCommunicationManager("UART Communication Proccess", 10240, 5);
    uart_communication_manager->InstallDriver(new UARTDriver(UART_NUM_0, Baudrate::BaudRate115200, 2048), ManagersAreaIndex::UART);

    // auto network_manager = new NetworkManager("Network Proccess", 10240, 4);
    // auto graphic_manager = new GraphicDriverManager("Graphic Process", 2048*2, 1);
    // auto lora_manager = new LoRaManager("Lora Process", 3072, 2);
    // auto http_server_manager = new HTTPServerManager("HTTP Server Process", 10240*2, 2);
    auto water_level_process = new WaterLevelProcess("Water Level Proccess", 1024, 5);

    uart_communication_manager->InitializeProcess();
    // network_manager->InitializeProcess();
    // graphic_manager->InitializeProcess();
    // lora_manager->InitializeProcess();
    // http_server_manager->InitializeProcess();

    water_level_process->InitializeProcess();

    return 0;
}

extern "C" void app_main(void) {
    main();
}