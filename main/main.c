#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "events.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "lcd1602.h"
#include "lcd_task.h"
#include "pir_task.h"

void app_main(void) {
    lcd_i2c_init();
    vTaskDelay(pdMS_TO_TICKS(100));
    lcd_init(I2C_NUM_0, 0x27, true);

    pir_gpio_init();

    event_queue = xQueueCreate(4, sizeof(pir_event_t));

    char initmsg[17] = " Initializing   ";
    char loading_bar[17] = "[              ]";

    ESP_LOGI("INIT", "Warmup");
    // 8s warm-up delay
    for (uint8_t i = 0; i < 15; i++) {
        if (i % 2 == 0)
            initmsg[14] = '|';
        if (i % 2 == 1)
            initmsg[14] = '-';

        loading_bar[i] = (i == 0) ? '[' : 0xFF;

        lcd_write(initmsg, loading_bar);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    xTaskCreate(pir_task, "pir_task", 2048, NULL, 1, NULL);
    xTaskCreate(lcd_task, "lcd_task", 4096, NULL, 1, NULL);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}