#include "memory/MemoryAreas/MemoryArea.hpp"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void test_memory_read_write() {
    auto result = ESP_FAIL;
    area_index_et index = DISPLAY_AREA;
    uint16_t size = 1024;
    uint32_t cached_time = 4096;
    auto area_read_write = AreaDef(READ_WRITE, index, size, cached_time);
    auto memory_area_instance = new MemoryArea(area_read_write);

    result = memory_area_instance->Clear();
    TEST_ASSERT_EQUAL(result, ESP_OK);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAccess(), READ_WRITE);
    TEST_ASSERT_EQUAL(memory_area_instance->GetSize(), size);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAreaIndex(), index);

    // Write and Read Area
    uint8_t expected_array[size] = {0};
    uint8_t expected_array_read[size] = {0};

    for (int i = 0; i < memory_area_instance->GetSize(); i++) {
        expected_array[i] = (uint8_t)(i % 256);
    }

    TEST_ASSERT_EQUAL(memory_area_instance->Write(expected_array, size), ESP_OK);
    TEST_ASSERT_EQUAL(memory_area_instance->Read(expected_array_read), ESP_OK);

    for (int i = 0; i < memory_area_instance->GetSize(); i++) {
        TEST_ASSERT_EQUAL(expected_array[i], expected_array_read[i]);
    }
}

void test_memory_read_only() {
    auto result = ESP_FAIL;
    area_index_et index = DISPLAY_AREA;
    uint16_t size = 1024;
    uint32_t cached_time = 4096;
    auto area_read_write = AreaDef(READ_ONLY, index, size, cached_time);
    auto memory_area_instance = new MemoryArea(area_read_write);

    result = memory_area_instance->Clear();
    TEST_ASSERT_EQUAL(result, ESP_OK);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAccess(), READ_ONLY);
    TEST_ASSERT_EQUAL(memory_area_instance->GetSize(), size);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAreaIndex(), index);

    // Read Area
    uint8_t expected_array_read[size] = {0};

    TEST_ASSERT_EQUAL(memory_area_instance->Write(expected_array_read, size), ESP_FAIL);
    TEST_ASSERT_EQUAL(memory_area_instance->Read(expected_array_read), ESP_OK);

    for (int i = 0; i < memory_area_instance->GetSize(); i++) {
        TEST_ASSERT_EQUAL(0, expected_array_read[i]);
    }
}

void test_memory_write_only() {
    auto result = ESP_FAIL;
    area_index_et index = DISPLAY_AREA;
    uint16_t size = 1024;
    uint32_t cached_time = 4096;
    auto area_read_write = AreaDef(WRITE_ONLY, index, size, cached_time);
    auto memory_area_instance = new MemoryArea(area_read_write);

    result = memory_area_instance->Clear();
    TEST_ASSERT_EQUAL(result, ESP_OK);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAccess(), WRITE_ONLY);
    TEST_ASSERT_EQUAL(memory_area_instance->GetSize(), size);
    TEST_ASSERT_EQUAL(memory_area_instance->GetAreaIndex(), index);

    // Write and Read Area
    uint8_t expected_array[size] = {0};

    TEST_ASSERT_EQUAL(memory_area_instance->Write(expected_array, size), ESP_OK);
    TEST_ASSERT_EQUAL(memory_area_instance->Read(expected_array), ESP_FAIL);

}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();
    RUN_TEST(test_memory_read_write);
    RUN_TEST(test_memory_read_only);
    RUN_TEST(test_memory_write_only);
    UNITY_END();
}

extern "C" void app_main(void) { main_test(); }