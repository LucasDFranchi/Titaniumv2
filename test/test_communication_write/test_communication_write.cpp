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
    uint8_t MOCK_AREA_INDEX    = 1;
    auto shared_memory_manager = SharedMemoryManager::GetInstance();

    /* Create a mock area just to store some data */
    kernel.SignUpSharedArea(MOCK_AREA_INDEX, ConnectionStatusProtobuf::GetStaticMaxSize(), READ_WRITE);

    ConnectionStatusProtobuf connection_status_proto{};
    connection_status_proto.UpdateApStatus(1);
    connection_status_proto.UpdateStaStatus(1);

    // uint8_t buffer[126] = 

    // CommunicationProtobuf communication_proto{};
    // communication_proto.UpdateAddress(0x1015);
    // communication_proto.UpdateCommand('R');
    // communication_proto.UpdateMemoryArea(MOCK_AREA_INDEX);
    // communication_proto.UpdatePayload()

    // /* I need to do a dummy write to initialize the area */
    // auto result = shared_memory_manager->Write(MOCK_AREA_INDEX, connection_status_proto);
    // TEST_ASSERT_EQUAL(0, result);

    connection_status_proto.UpdateApStatus(10);
    connection_status_proto.UpdateStaStatus(25);

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
    std::unique_ptr<TitaniumPackage> package = std::make_unique<TitaniumPackage>(0, WRITE_COMMAND, MOCK_AREA_INDEX, connection_status_proto);
    /* Convert the package to a buffer */
    auto written_bytes = protocol.Encode(package, buffer, sizeof(buffer));
    TEST_ASSERT_GREATER_THAN(0, written_bytes);

    /* Mock transmitted data written directly into the driver buffer */
    TEST_ASSERT_EQUAL(ESP_OK, driver->Write((uint8_t*)buffer, written_bytes));

    /* Wait some time to process the received package */
    vTaskDelay(pdMS_TO_TICKS(2000));

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