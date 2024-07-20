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

void test_Write() {
    Kernel kernel;
    TitaniumProtocol protocol;
    uint8_t MOCK_AREA_INDEX    = 2;
    auto shared_memory_manager = SharedMemoryManager::GetInstance();

    /* Create a mock area just to store some data */
    kernel.SignUpSharedArea(MOCK_AREA_INDEX, ConnectionStatusProtobuf::GetStaticMaxSize(), READ_WRITE);

    ConnectionStatusProtobuf connection_status_proto{};
    connection_status_proto.UpdateApStatus(2);
    connection_status_proto.UpdateStaStatus(3);

    /* Create a mock driver */
    uint8_t buffer[1024] = {0};
    uint16_t buffer_size = 1024;
    auto driver          = new TestDriver(buffer_size);

    /* Start mock process */
    uint16_t address           = 0x1015;
    auto communication_process = new CommunicationProcess("Test Communication Proccess", 40240, 5);
    communication_process->InstallDriver(driver,
                                         ProtobufIndex::LORA_TRANSMIT,
                                         ProtobufIndex::LORA_RECEIVE);
    communication_process->Configure(address);
    communication_process->InitializeProcess();

    /* Create a mock package to be received by the application */
    std::unique_ptr<TitaniumPackage> package = std::make_unique<TitaniumPackage>(0, WRITE_COMMAND, MOCK_AREA_INDEX, connection_status_proto);
    /* Convert the package to a buffer */
    auto written_bytes = protocol.Encode(package, buffer, sizeof(buffer));
    TEST_ASSERT_GREATER_THAN(0, written_bytes);

    /* Mock transmitted data written directly into the driver buffer */
    TEST_ASSERT_EQUAL(ESP_OK, driver->Write((uint8_t*)buffer, written_bytes));

    /* Wait some time to process the received package */
    vTaskDelay(pdMS_TO_TICKS(200));

    ConnectionStatusProtobuf connection_status_proto_read{};
    auto result = shared_memory_manager->Read(MOCK_AREA_INDEX, connection_status_proto_read);
    TEST_ASSERT_EQUAL(0, result);

    TEST_ASSERT_EQUAL(connection_status_proto.GetApStatus(), connection_status_proto_read.GetApStatus());
    TEST_ASSERT_EQUAL(connection_status_proto.GetStaStatus(), connection_status_proto_read.GetStaStatus());
}

void main_test(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    UNITY_BEGIN();

    RUN_TEST(test_Write);

    UNITY_END();
}

extern "C" void app_main(void) {
    main_test();
}