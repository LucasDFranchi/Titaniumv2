#include "nvs_flash.h"

#include <HAL/gpio/GPIOManager.h>
#include <HAL/memory/SharedMemoryManager.h>
#include <HAL/spi/SPIManager.h>

#include "Drivers/LoRa/LoRaDriver.h"
#include "Drivers/UART/UARTDriver.h"

#include <Managers/HTTPServerManager/inc/HTTPServerManager.h>
#include <Managers/NetworkManager/inc/NetworkManager.h>
#include <Managers/SerialCommunicationManager/inc/SerialCommunicationManager.h>

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
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::UART_RECEIVE, 256, READ_WRITE));
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::UART_TRANSMIT, 256, READ_WRITE));
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::LORA_RECEIVE, 256, READ_WRITE));
    ESP_ERROR_CHECK(shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::LORA_TRANSMIT, 256, READ_WRITE));

    credentials_st credentials_debug;
    const char* ssid     = "SSID";
    const char* password = "PASSWORD";
    memcpy_s<uint8_t>(credentials_debug.sta_ssid, (uint8_t*)ssid, sizeof(credentials_debug.sta_ssid));
    memcpy_s<uint8_t>(credentials_debug.sta_password, (uint8_t*)password, sizeof(credentials_debug.sta_password));
    shared_memory_manager->Write(ManagersAreaIndex::CREDENTIALS, sizeof(credentials_st), &credentials_debug);

    auto uart_communication_manager = new SerialCommunicationManager("UART Communication Proccess", 10240, 5);
    uart_communication_manager->InstallDriver(new UARTDriver(UART_NUM_0, Baudrate::BaudRate115200, 1024), ManagersAreaIndex::UART_RECEIVE, ManagersAreaIndex::UART_TRANSMIT);
    auto lora_communication_manager = new SerialCommunicationManager("LoRa Communication Proccess", 10240, 4);
    lora_communication_manager->InstallDriver(new LoRaDriver(Regions::BRAZIL, CRCMode::DISABLE, 255), ManagersAreaIndex::LORA_RECEIVE, ManagersAreaIndex::LORA_TRANSMIT);

    auto network_manager     = new NetworkManager("Network Proccess", 10240, 4);
    auto http_server_manager = new HTTPServerManager("HTTP Server Process", 10240 * 2, 2);

    uart_communication_manager->InitializeProcess();
    lora_communication_manager->InitializeProcess();
    network_manager->InitializeProcess();
    http_server_manager->InitializeProcess();

    return 0;
}

extern "C" void app_main(void) {
    main();
}