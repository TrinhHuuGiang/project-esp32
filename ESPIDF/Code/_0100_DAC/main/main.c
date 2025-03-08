#include <stdio.h>
#include "driver/dac.h"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DAC_CHANNEL DAC_CHANNEL_1 // GPIO25
#define SAMPLE_RATE 44100
#define SAMPLE_PERIOD_US (1000000 / SAMPLE_RATE)

extern const uint8_t melody[];
extern int melody_size;

volatile int index = 0; // Chỉ số của mẫu âm thanh

// ISR của Timer Group
void IRAM_ATTR play_sample(void *arg) {
    if (index < melody_size) {
        dac_output_voltage(DAC_CHANNEL, melody[index++]);
    } else {
        index = 0; // Lặp lại từ đầu
    }
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
}

void app_main() {
    dac_output_enable(DAC_CHANNEL);

    // Cấu hình Timer Group
    timer_config_t config = {
        .divider = 80, // 80MHz / 80 = 1MHz (1µs)
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, SAMPLE_PERIOD_US);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, play_sample, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
}
