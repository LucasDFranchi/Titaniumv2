#include "Kernel/Kernel.h"

#include "CustomProcess/WaterLevelProcess/inc/WaterLevelProcess.h"

int main(void) {
    Kernel kernel(HealthMonitor::ENABLE_MONITORING);

    kernel.EnableNetworkProcess(4096, 4);
    kernel.EnableHTTPServerProcess(2048, 2);
    kernel.EnableUartProcess(2048, 5);
    kernel.EnableLoraProcess(2048, 5);
    kernel.EnableMQTTClientProcess(4096, 5);

    auto water_level_process = new WaterLevelProcess("Water Level Process", 2048, 2);
    water_level_process->InitializeProcess();
    
    kernel.SignUpSharedArea(ProtobufIndex::WATER_LEVEL, WaterLevelProtobuf::GetStaticMaxSize(), READ_WRITE);
    kernel.MonitorProcess(water_level_process);

    kernel.InjectDebugCredentials("NETPARQUE_PAOLA", "NPQ196253");

    return 0;   
}

extern "C" void app_main(void) {
    main();
}