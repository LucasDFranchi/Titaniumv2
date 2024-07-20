#include "Protocols/Titanium/TitaniumPackage.h"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void test_package_creation() {
    uint8_t buffer[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    command_e command = WRITE_COMMAND;
    uint16_t address = 0x4321;
    uint16_t size = sizeof(buffer);
    uint8_t memory_area = 1;

    auto package_data = new TitaniumPackage(size, address, command, memory_area, buffer);
    
    TEST_ASSERT_EQUAL(package_data->size(), size);
    TEST_ASSERT_EQUAL(package_data->command(), command);
    TEST_ASSERT_EQUAL(package_data->memory_area(), memory_area);
    TEST_ASSERT_EQUAL(package_data->address(), address);
    
    uint8_t buffer_consumed[16] = {0};
    package_data->Consume(buffer_consumed);

    for (int i = 0; i < size; i++){
        TEST_ASSERT_EQUAL(buffer[i], buffer_consumed[i]);
    }
}

void test_package_decoupling() {
    uint8_t buffer[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t buffer_backup[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    command_e command = WRITE_COMMAND;
    uint16_t size = sizeof(buffer);
    uint8_t memory_area = 1;

    auto package_data = new TitaniumPackage(size, 0x0000, command, memory_area, buffer);
    
    TEST_ASSERT_EQUAL(package_data->size(), size);
    TEST_ASSERT_EQUAL(package_data->command(), command);
    TEST_ASSERT_EQUAL(package_data->memory_area(), memory_area);
    
    uint8_t buffer_consumed[16] = {0};
    package_data->Consume(buffer_consumed);

    for (int i = 0; i < size; i++){
        TEST_ASSERT_EQUAL(buffer[i], buffer_consumed[i]);
    }

    for (int i = 0; i < size; i++) {
        buffer[i] = i*4;
    }

    for (int i = 0; i < size; i++){
        TEST_ASSERT_EQUAL(buffer_backup[i], buffer_consumed[i]);
    }

}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();
    RUN_TEST(test_package_creation);
    RUN_TEST(test_package_decoupling);
    UNITY_END();
}

extern "C" void app_main(void) { main_test(); }