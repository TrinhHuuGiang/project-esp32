# ledc
- ![common option](./img/_0405_common_ledc_freq_resolution.png)
- source: esp32 refernce manual
- demo: ![rgb ledc](../Demo/_0405_ledc.mp4)

# mcpwm
- esp32 có 2 đơn vị mcpwm
- mỗi đơn vị có 3 timer
- mỗi timer điều khiển 2 đầu ra A và B
- mỗi timer của mcpwm 8bit chia tần số prescaler
- thanh ghi đếm 16bit có thể đếm thuận và đảo
- 