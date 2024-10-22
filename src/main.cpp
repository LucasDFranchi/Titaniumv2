#include "Application/Application.h"

#include "CustomProcess/TimeProcess/inc/TimeProcess.h"

int main(void) {
    Application app;

    app.EnableNetworkProcess(10240, 4);
    app.EnableHTTPServerProcess(10240, 2);
    app.EnableUartProcess(10240, 5);
    app.EnableLoraProcess(20240, 5, true);
    app.EnableMQTTClientProcess(10240, 5);

    app.InjectDebugCredentials("NETPARQUE_PAOLA", "NPQ196253");

    // app.SignUpSharedArea(MEMORY_AREAS_TIME_PROCESS, sizeof(time_process_t), READ_WRITE);
    // auto time_process = new TimeProcess("Time Proccess", 2048, 5);
    // time_process->InitializeProcess();

    continuos_packet_list_t continuos_packet_list{};
    continuos_packet_list.packet_configs_count = 1;
    continuos_packet_list.packet_configs[0] = {
        .destination_address = 0x0000,
        .destination_area    = MEMORY_AREAS_TIME_PROCESS,
        .requested_area      = MEMORY_AREAS_TIME_PROCESS,
        .packet_interval     = 5000000,
        .last_transmission   = 0,
    };

    app.InjectMemoryAreaData(MEMORY_AREAS_LORA_CONTINUOS_PACKET,
                             continuos_packet_list,
                             continuos_packet_list_t_msg);

    return 0;
}

extern "C" void app_main(void) {
    main();
}