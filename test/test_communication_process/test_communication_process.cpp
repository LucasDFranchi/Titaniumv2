#include "Kernel/Kernel.h"
#include "Protocols/Titanium/TitaniumProtocol.h"
#include "TestDriver.h"

#include "esp_Log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_ProcessInitialization() {
    Kernel kernel;
    TitaniumProtocol protocol;
    auto receive_proto     = CommunicationProtobuf{};
    uint8_t buffer[1024]   = {0};
    uint8_t data_buffer[2] = {2, 3};
    uint16_t buffer_size   = 1024;
    auto driver            = new TestDriver(buffer_size);

    std::unique_ptr<TitaniumPackage> package = std::make_unique<TitaniumPackage>(sizeof(data_buffer), 0, READ_COMMAND, 1, data_buffer);
    auto written_bytes                       = protocol.Encode(package, buffer, sizeof(buffer));
    TEST_ASSERT_GREATER_THAN(0, written_bytes);
    ESP_LOGI("TEST", "Written Bytes: %d", written_bytes);
    kernel.SignUpSharedArea(1, 2, READ_WRITE);

    for (uint16_t i = 0; i < written_bytes; i++) {
        ESP_LOGI("TEST", "Byte Write %d: %x", i, buffer[i]);
    }

    auto communication_process = new CommunicationProcess("Test Communication Proccess", 40240, 5);
    communication_process->InstallDriver(driver);
    communication_process->Configure(0x1015, 1);
    communication_process->InitializeProcess();

    TEST_ASSERT_EQUAL(ESP_OK, communication_process->driver()->Write((uint8_t*)buffer, written_bytes));
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI("TEST", "Address: Start Desearilze");
    for (uint16_t i = 0; i < 200; i++) {
        ESP_LOGI("TEST", "Byte Read %d: %x", i, driver->buffer[i]);
    }

    TEST_ASSERT_EQUAL(ESP_OK, protocol.Decode(driver->buffer, sizeof(driver->buffer), package));
    package.get()->Consume(buffer);
    TEST_ASSERT_EQUAL(ESP_OK, receive_proto.DeSerialize((char*)buffer, buffer_size));

    ESP_LOGI("TEST", "Address: %d", receive_proto.GetAddress());
    ESP_LOGI("TEST", "Command: %d", receive_proto.GetCommand());
    ESP_LOGI("TEST", "Memory Area: %d", receive_proto.GetMemoryArea());
    ESP_LOGI("TEST", "Payload: %s", receive_proto.GetPayload());

    TEST_ASSERT_EQUAL(receive_proto.GetAddress(), 0x1015);
    TEST_ASSERT_EQUAL(receive_proto.GetCommand(), READ_RESPONSE_COMMAND);
    TEST_ASSERT_EQUAL(receive_proto.GetMemoryArea(), 4);
    TEST_ASSERT_EQUAL(receive_proto.GetPayload(), "This is a message to be stored!");
}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();

    RUN_TEST(test_ProcessInitialization);

    UNITY_END();
}

extern "C" void app_main(void) {
    main_test();
}