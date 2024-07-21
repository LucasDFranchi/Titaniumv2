#include "Kernel/Kernel.h"

#include "Protocols/Protobuf/inc/WaterLevelProto.h"

int main(void) {
    Kernel kernel;

    kernel.EnableNetworkProcess(10240, 4);
    kernel.EnableHTTPServerProcess(20480, 2);
    kernel.EnableUartProcess(10240, 5);
    kernel.EnableLoraProcess(10240, 5);
    kernel.EnableMQTTClientProcess(10240, 5);
    
    kernel.SignUpSharedArea(ProtobufIndex::END_DEVICE_00, WaterLevelProtobuf::GetStaticMaxSize(), READ_WRITE);
    kernel.SignUpSharedArea(ProtobufIndex::END_DEVICE_01, WaterLevelProtobuf::GetStaticMaxSize(), READ_WRITE);
    kernel.SignUpSharedArea(ProtobufIndex::END_DEVICE_02, WaterLevelProtobuf::GetStaticMaxSize(), READ_WRITE);
    kernel.SignUpSharedArea(ProtobufIndex::END_DEVICE_03, WaterLevelProtobuf::GetStaticMaxSize(), READ_WRITE);
    kernel.SignUpSharedArea(ProtobufIndex::END_DEVICE_04, WaterLevelProtobuf::GetStaticMaxSize(), READ_WRITE);

    kernel.InjectDebugCredentials("ssid", "password");

    return 0;   
}

extern "C" void app_main(void) {
    main();
}