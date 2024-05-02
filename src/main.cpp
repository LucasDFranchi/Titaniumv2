#include "nvs_flash.h"

// #include <memory/MemoryManager.hpp>
// #include <gpio/GPIOManager.h>
// #include <spi/SPIManager.h>

// #include <CommandManager.h>
// #include <GraphicDriverManager.h>
// #include <SerialDriverManager.h>
// #include <LoRaManager.h>
// #include <NetworkManager.h>
// #include <HTTPServerManager.h>

// MemoryManager* MemoryManager::singleton_pointer_ = nullptr;
// GPIOManager*   GPIOManager::singleton_pointer_ = nullptr;
// SPIManager*    SPIManager::singleton_pointer_ = nullptr;

#include <iostream>
#include <memory>

class MyClass {
   public:
    MyClass(int value) : data(value) {
        std::cout << "Constructor called with value: " << value << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor called for value: " << data << std::endl;
    }
    void display() { std::cout << "Data: " << data << std::endl; }

   private:
    int data;
};

esp_err_t initialize_nvs(void) {
    nvs_flash_erase();
    esp_err_t ret = nvs_flash_init();
    return ret;
}

int main(void) {
    initialize_nvs();

    // Using unique_ptr with make_unique
    auto uniquePtr = std::make_unique<MyClass>(42);
    uniquePtr->display();

    // Using shared_ptr with make_shared
    auto sharedPtr1 = std::make_shared<MyClass>(10);
    auto sharedPtr2 = sharedPtr1;
    sharedPtr1->display();
    sharedPtr2->display();

    // Using weak_ptr with make_shared
    auto sharedPtr3 = std::make_shared<MyClass>(99);
    std::weak_ptr<MyClass> weakPtr = sharedPtr3;

    if (auto sharedPtr4 = weakPtr.lock()) {
        std::cout << "Weak pointer is valid." << std::endl;
        sharedPtr4->display();
    } else {
        std::cout << "Weak pointer is expired." << std::endl;
    }

    // auto memory_manager = MemoryManager::GetInstance();
    // auto gpio_manager = GPIOManager::GetInstance();
    // auto spi_manager = SPIManager::GetInstance();

    // memory_manager->Initialize();
    // gpio_manager->Initialize();
    // spi_manager->Initialize();

    // gpio_manager->WriteGPIO(LED_WHITE, HIGH);

    // auto serial_manager = new SerialDriverManager("Serial Proccess", 10240,
    // 5); auto network_manager = new NetworkManager("Network Proccess", 10240,
    // 4); auto graphic_manager = new GraphicDriverManager("Graphic Process",
    // 2048*2, 1); auto lora_manager = new LoRaManager("Lora Process", 3072, 2);
    // auto http_server_manager = new HTTPServerManager("HTTP Server Process",
    // 10240*2, 2);

    // serial_manager->InitializeProcess();
    // network_manager->InitializeProcess();
    // graphic_manager->InitializeProcess();
    // lora_manager->InitializeProcess();
    // http_server_manager->InitializeProcess();

    return 0;
}

extern "C" void app_main(void) { main(); }