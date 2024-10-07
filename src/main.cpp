#include "Application/Application.h"

#include "CustomProcess/TimeProcess/inc/TimeProcess.h"

int main(void) {
    Application app;

    app.EnableNetworkProcess(10240, 4);
    app.EnableHTTPServerProcess(10240, 2);
    app.EnableUartProcess(10240, 5);
    app.EnableLoraProcess(20240, 5, true);
    app.EnableMQTTClientProcess(10240, 5);

    return 0;
}

extern "C" void app_main(void) {
    main();
}