#include "CommandBlink.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

CommandBlink::CommandBlink(int blink_time) { this->blink_time = blink_time; }

void CommandBlink::Setup(void) {
    gpio_reset_pin(GPIO_NUM_25);
    gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_25, 0);
}

bool CommandBlink::Loop(void) {
    bool ret = false;
    int state = this->counter % 2 == 0;
    gpio_set_level(GPIO_NUM_25, state);
    vTaskDelay(100);
    if (this->counter++ > 60) {
        ret = true;
    }

    return ret;
}

void CommandBlink::Teardown(void) { gpio_set_level(GPIO_NUM_25, 0); }