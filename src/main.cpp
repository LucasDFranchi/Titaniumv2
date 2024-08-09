#include "Kernel/Kernel.h"

#include "CustomProcess/WaterLevelProcess/inc/WaterLevelProcess.h"

int main(void) {
    Kernel kernel;

    kernel.EnableNetworkProcess(10240, 4);
    kernel.EnableHTTPServerProcess(20480, 2);
    kernel.EnableUartProcess(10240, 5);
    kernel.EnableLoraProcess(10240, 5, true);
    kernel.EnableMQTTClientProcess(10240, 5);

    // auto water_level_process = new WaterLevelProcess("Water Level Process", 10240, 2);
    // water_level_process->InitializeProcess();
    
    // kernel.SignUpSharedArea(ProtobufIndex::WATER_LEVEL, WaterLevelProtobuf::GetStaticMaxSize(), READ_WRITE);

    // kernel.InjectDebugCredentials("NETPARQUE_PAOLA", "NPQ196253");

    return 0;   
}

extern "C" void app_main(void) {
    main();
}