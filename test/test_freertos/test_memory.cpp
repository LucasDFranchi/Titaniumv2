#include <unity.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void test_memory_heap_allocation(void *arg){
    UBaseType_t uxHighWaterMark;

    /* Inspect our own high water mark on entering the task. */
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    printf("Initial available memory: %d", uxHighWaterMark);
    for( ;; )
    {
        /* Call any function. */
        vTaskDelay( 1000 );

        /* Calling the function will have used some stack space, we would 
        therefore now expect uxTaskGetStackHighWaterMark() to return a 
        value lower than when it was called on entering the task. */
        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    }
}

int main(void)
{
    xTaskCreate(test_memory_heap_allocation,
                "test_memory_heap_allocation",
                1024,
                NULL,
                1,
                NULL);
    return 0;
}

extern "C" void app_main(void) { main(); }