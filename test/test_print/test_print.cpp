#include "PrintUtils.h"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void test_secure_snprintf() {
    char buffer[20] = {0};
    uint32_t result = snprintf_s(buffer, sizeof(buffer), "%s", "Hello, World!");
    
    TEST_ASSERT_EQUAL(result, 13);
    TEST_ASSERT_EQUAL_STRING(buffer, "Hello, World!");
    TEST_ASSERT_EQUAL(buffer[result], '\0');
}

void test_secure_snprintf_truncate() {
    char buffer[10];
    uint32_t result = snprintf_s(buffer, sizeof(buffer), "%s", "Hello, World!");
    
    TEST_ASSERT_EQUAL(result, 13);
    TEST_ASSERT_EQUAL_STRING(buffer, "Hello, Wo\0");
    TEST_ASSERT_EQUAL(buffer[sizeof(buffer) - 1], '\0');
}

void test_secure_snprintf_nullptr() {
    uint32_t result = snprintf_s(nullptr, 512, "%s", "Hello, World!");
    
    TEST_ASSERT_EQUAL(result, 0);
}

void test_secure_snprintf_array() {
    uint8_t array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    char output_buffer[35] = {0};

    uint32_t result = snprintf_array(output_buffer, array, sizeof(array), sizeof(output_buffer));
    TEST_ASSERT_EQUAL_STRING(output_buffer, "[0,1,2,3,4,5,6,7,8,9]\0");
    TEST_ASSERT_EQUAL(result, 21);
}

void test_secure_snprintf_array_truncate() {
    uint8_t array[17] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6};
    char output_buffer[36] = {0};

    uint32_t result = snprintf_array(output_buffer, array, sizeof(array), sizeof(output_buffer));
    TEST_ASSERT_EQUAL_STRING(output_buffer, "[0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6]");
    TEST_ASSERT_EQUAL(result, 35);
}

void test_secure_snprintf_array_nullptr() {
    uint8_t array[17] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6};
    char output_buffer[36] = {0};

    uint32_t result = snprintf_array(nullptr, array, sizeof(array), sizeof(output_buffer));
    TEST_ASSERT_EQUAL_STRING(output_buffer, "\0");
    TEST_ASSERT_EQUAL(result, 0);

    result = snprintf_array(nullptr, nullptr, sizeof(array), sizeof(output_buffer));
    TEST_ASSERT_EQUAL_STRING(output_buffer, "\0");
    TEST_ASSERT_EQUAL(result, 0);

    result = snprintf_array(output_buffer, nullptr, sizeof(array), sizeof(output_buffer));
    TEST_ASSERT_EQUAL_STRING(output_buffer, "\0");
    TEST_ASSERT_EQUAL(result, 0);
}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();
    RUN_TEST(test_secure_snprintf);
    RUN_TEST(test_secure_snprintf_truncate);
    RUN_TEST(test_secure_snprintf_nullptr);
    RUN_TEST(test_secure_snprintf_array);
    RUN_TEST(test_secure_snprintf_array_truncate);
    RUN_TEST(test_secure_snprintf_array_nullptr);
    UNITY_END();
}

extern "C" void app_main(void) { main_test(); }