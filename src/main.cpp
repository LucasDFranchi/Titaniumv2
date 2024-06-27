#include "Kernel/Kernel.h"

int main(void) {
    Kernel Kernel;

    
    Kernel.InitializeNetwork(10240, 4);
    Kernel.InitializeHTTPServer(20480, 2);
    Kernel.InitializeUart(10240, 5);
    Kernel.InitializeLora(10240, 5);

    Kernel.InjectDebugCredentials("NETPARQUE_PAOLA", "NPQ196253");


    return 0;
}

extern "C" void app_main(void) {
    main();
}