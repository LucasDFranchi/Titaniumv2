#include "../../include/VectorManager.hpp"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_LimitSizeVector() {
    int max_size = 100;

    VectorManager<int> vector_manager(max_size);

    for(int i = 0; i < max_size; i++){
        TEST_ASSERT_EQUAL(ESP_OK, vector_manager.push_back(i));
    }
    TEST_ASSERT_EQUAL(ESP_FAIL, vector_manager.push_back(101));

}
void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();

    RUN_TEST(test_LimitSizeVector);

    UNITY_END();
}

extern "C" void app_main(void) { main_test(); }