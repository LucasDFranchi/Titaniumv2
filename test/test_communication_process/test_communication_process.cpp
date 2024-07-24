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
    uint8_t MOCK_AREA_INDEX    = 1;
    auto shared_memory_manager = SharedMemoryManager::GetInstance();

    /* Create a mock area just to store some data */
    kernel.SignUpSharedArea(MOCK_AREA_INDEX, ConnectionStatusProtobuf::GetStaticMaxSize(), READ_WRITE);

    ConnectionStatusProtobuf connection_status_proto{};
    connection_status_proto.UpdateApStatus(2);
    connection_status_proto.UpdateStaStatus(3);

    shared_memory_manager->Write(MOCK_AREA_INDEX, &connection_status_proto);

    /* Create a mock driver */
    uint8_t buffer[1024] = {0};
    uint16_t buffer_size = 1024;
    auto driver          = new TestDriver(buffer_size);

    /* Start mock process */
    uint16_t address           = 0x1015;
    auto communication_process = new CommunicationProcess("Test Communication Proccess", 40240, 5);
    communication_process->InstallDriver(driver);
    communication_process->Configure(address);
    communication_process->InitializeProcess();

    /* Create a mock package to be received by the application */
    std::unique_ptr<TitaniumPackage> package = std::make_unique<TitaniumPackage>(0, READ_COMMAND, MOCK_AREA_INDEX, connection_status_proto);
    /* Convert the package to a buffer */
    auto written_bytes = protocol.Encode(package, buffer, sizeof(buffer));
    TEST_ASSERT_GREATER_THAN(0, written_bytes);

    /* Mock transmitted data written directly into the driver buffer */
    TEST_ASSERT_EQUAL(ESP_OK, driver->Write((uint8_t*)buffer, written_bytes));

    /* Wait some time to process the received package */
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* At this point we're reading the buffer of the device that start the transmission with the READ_RESPONSE_COMMAND*/
    TEST_ASSERT_EQUAL(ESP_OK, protocol.Decode(driver->buffer, sizeof(driver->buffer), package));  // We reuse the package

    /* Since this buffer now belongs to the transmitter we should clean before doing anything */
    memset(buffer, 0, sizeof(buffer));
    auto received_bytes = package.get()->Consume(buffer);
    TEST_ASSERT_GREATER_THAN(0, received_bytes);

    /* Deserialize the received buffer */
    auto receive_proto = CommunicationProtobuf{};
    TEST_ASSERT_EQUAL(ESP_OK, receive_proto.DeSerialize((char*)buffer, buffer_size));

    /* Compare */
    TEST_ASSERT_EQUAL(receive_proto.GetAddress(), 0x1015);
    TEST_ASSERT_EQUAL(receive_proto.GetCommand(), READ_RESPONSE_COMMAND);
    TEST_ASSERT_EQUAL(receive_proto.GetMemoryArea(), 4);
    TEST_ASSERT_EQUAL_STRING(receive_proto.GetPayload(), "{\n    \"ap_status\": 2,\n    \"sta_status\": 3\n}");
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