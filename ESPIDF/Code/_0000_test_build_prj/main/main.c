#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "stdio.h"

void app_main() {
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1) {
        printf("Chạy mỗi 2 giây\n");
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(2000));  // Chính xác mỗi 2 giây
    }
}
