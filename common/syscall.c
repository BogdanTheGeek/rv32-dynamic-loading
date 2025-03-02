

#include "syscall.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

void delay_ms(size_t ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }

sys_t sys = {
    .delay_Ms = delay_ms,
    .log = printf,
};

