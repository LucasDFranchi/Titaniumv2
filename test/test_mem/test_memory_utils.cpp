#include "HAL/memory/MemoryHandlers.h"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}


void test_MemoryCopyOK() {
    auto buffer_size = 10;
    uint8_t test_bytes[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t test_bytes_expected[] = {0x02, 0x05, 0x00,'W', 0x01, 'L', 'U', 'C', 'A', 'S'};

    TEST_ASSERT_EQUAL(memcpy_s<uint8_t>(test_bytes, test_bytes_expected, buffer_size), ESP_OK);

    for (int i = 0; i < buffer_size; i++){
        TEST_ASSERT_EQUAL(test_bytes[i], test_bytes_expected[i]);
    }
}

void test_MemoryCopySizeZero() {
    uint8_t test_bytes[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t test_bytes_expected[] = {0x02, 0x05, 0x00,'W', 0x01, 'L', 'U', 'C', 'A', 'S'};
    TEST_ASSERT_EQUAL(memcpy_s<uint8_t>(test_bytes, test_bytes_expected, 0), ESP_ERR_INVALID_SIZE);
}

void test_MemoryCopyWithNullStart() {
    auto buffer_size = 10;
    uint8_t test_bytes[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t test_bytes_expected[] = {0x02, 0x05, 0x00,'W', 0x01, 'L', 'U', 'C', 'A', 'S'};
    TEST_ASSERT_EQUAL(memcpy_s<uint8_t>(test_bytes, NULL, buffer_size), ESP_ERR_NO_MEM);
    TEST_ASSERT_EQUAL(memcpy_s<uint8_t>(NULL, test_bytes_expected, buffer_size), ESP_ERR_NO_MEM);
}

void test_MemorySetOK() {
    auto buffer_size = 10;
    uint8_t test_bytes[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

    TEST_ASSERT_EQUAL(memset_s<uint8_t>(test_bytes, 0, buffer_size), ESP_OK);

    for (int i = 0; i < sizeof(test_bytes); i++){
        TEST_ASSERT_EQUAL(test_bytes[i], 0);
    }
}

void test_MemorySetSizeZero() {
    uint8_t test_bytes[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL(memset_s<uint8_t>(test_bytes, 0, 0), ESP_ERR_INVALID_SIZE);
}

void test_MemorySetWithNullStart() {
    auto buffer_size = 10;
    TEST_ASSERT_EQUAL(memset_s<uint8_t>(nullptr, 0, buffer_size), ESP_ERR_NO_MEM);
}


void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();

    RUN_TEST(test_MemoryCopyOK);

    RUN_TEST(test_MemoryCopySizeZero);
    RUN_TEST(test_MemoryCopyWithNullStart);
    RUN_TEST(test_MemorySetOK);
    RUN_TEST(test_MemorySetSizeZero);
    RUN_TEST(test_MemorySetWithNullStart);

    UNITY_END();
}

extern "C" void app_main(void) { main_test(); }