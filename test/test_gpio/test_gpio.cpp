#include "GPIOManager.h"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

GPIOManager*   GPIOManager::singleton_pointer_ = nullptr;

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_gpio_read_write() {
    auto gpio_manager = GPIOManager::GetInstance();
    gpio_manager->Initialize();

    TEST_ASSERT_EQUAL(gpio_manager->WriteGPIO(LED_WHITE, HIGH), ESP_OK);
    TEST_ASSERT_EQUAL(gpio_get_level(GPIO_NUM_25), 1);
    TEST_ASSERT_EQUAL(gpio_manager->ReadGPIO(LED_WHITE), 1);
    TEST_ASSERT_EQUAL(gpio_manager->WriteGPIO(LED_WHITE, LOW), ESP_OK);
    TEST_ASSERT_EQUAL(gpio_get_level(GPIO_NUM_25), 0);
    TEST_ASSERT_EQUAL(gpio_manager->ReadGPIO(LED_WHITE), 0);
}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();

    RUN_TEST(test_gpio_read_write);

    UNITY_END();
}

extern "C" void app_main(void) { main_test(); }