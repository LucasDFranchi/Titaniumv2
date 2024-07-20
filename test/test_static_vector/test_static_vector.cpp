#include "Libraries/DataContainers/inc/StaticVector.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

#include "esp_log.h"

void setUp(void) {
    // This function will be called before each test.
}

void tearDown(void) {
    // This function will be called after each test.
}

void test_StaticVector_Initialization(void) {
    StaticVector<int> vec(5);
    TEST_ASSERT_EQUAL(0, vec.push_back(10));  // Should succeed
    TEST_ASSERT_EQUAL(10, vec.back());        // Should be 10
    TEST_ASSERT_EQUAL(0, vec.push_back(20));  // Should succeed
    TEST_ASSERT_EQUAL(20, vec.back());        // Should be 20
}

void test_StaticVector_PushBack(void) {
    StaticVector<int> vec(3);
    TEST_ASSERT_EQUAL(0, vec.push_back(1));
    TEST_ASSERT_EQUAL(0, vec.push_back(2));
    TEST_ASSERT_EQUAL(0, vec.push_back(3));
    TEST_ASSERT_EQUAL(-1, vec.push_back(4));
    TEST_ASSERT_EQUAL(3, vec.back());
}

void test_StaticVector_Back(void) {
    StaticVector<int> vec(2);
    vec.push_back(5);
    vec.push_back(6);
    TEST_ASSERT_EQUAL(6, vec.back());  // Should be 6
}

void test_StaticVector_PopBack_Empty(void) {
    StaticVector<int> vec(2);
    vec.push_back(9);
    vec.pop_back();
    vec.pop_back();
}

void test_StaticVector_MaxSize(void) {
    StaticVector<int> vec(5);

    // Add elements up to the maximum size
    TEST_ASSERT_EQUAL(0, vec.push_back(1));
    TEST_ASSERT_EQUAL(0, vec.push_back(2));
    TEST_ASSERT_EQUAL(0, vec.push_back(3));
    TEST_ASSERT_EQUAL(0, vec.push_back(4));
    TEST_ASSERT_EQUAL(0, vec.push_back(5));

    TEST_ASSERT_EQUAL(-1, vec.push_back(6));
    TEST_ASSERT_EQUAL(5, vec.back());
}

void test_StaticVector_HeapUsage(void) {
    StaticVector<uint32_t> vec(1024);
    size_t initial_free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    size_t free_heap = 0;
    for (int i = 0; i < 1024; ++i) {
        free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
        TEST_ASSERT_EQUAL(0, vec.push_back(i));
    }
    auto delta_heap = initial_free_heap - free_heap;
    TEST_ASSERT_TRUE(delta_heap == (1024 * sizeof(uint32_t))); 
}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();

    RUN_TEST(test_StaticVector_Initialization);
    RUN_TEST(test_StaticVector_PushBack);
    RUN_TEST(test_StaticVector_Back);
    RUN_TEST(test_StaticVector_PopBack_Empty);
    RUN_TEST(test_StaticVector_MaxSize);
    RUN_TEST(test_StaticVector_HeapUsage);

    UNITY_END();
}

extern "C" void app_main(void) {
    main_test();
}