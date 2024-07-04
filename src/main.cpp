#include "Kernel/Kernel.h"

#include "CustomProcess/WaterLevelProcess/WaterLevelProcess.h"

int main(void) {
    Kernel kernel;

    kernel.EnableNetworkProcess(10240, 4);
    kernel.EnableHTTPServerProcess(20480, 2);
    kernel.EnableUartProcess(10240, 5);
    kernel.EnableLoraProcess(10240, 5);

    auto water_level_process = new WaterLevelProcess("Water Level Process", 1024, 2);
    water_level_process->InitializeProcess();

    // kernel.InjectDebugCredentials("NETPARQUE_PAOLA", "NPQ196253");

    return 0;
}

extern "C" void app_main(void) {
    main();
}