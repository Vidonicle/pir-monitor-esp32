#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "events.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hardware.h"
#include "lcd1602.h"

void app_main(void) {
    lcd_i2c_init();
    vTaskDelay(pdMS_TO_TICKS(100));
    lcd_init(I2C_NUM_0, 0x27, true);

    pir_gpio_init();

    lcd_queue = xQueueCreate(4, sizeof(pir_event_t));

    ESP_LOGI("INIT", "Warming up");
    lcd_write("INIALIZING", ":3");
    vTaskDelay(pdMS_TO_TICKS(30000));  // 30s warm-up delay

    xTaskCreate(pir_task, "pir_task", 2048, NULL, 1, NULL);
    xTaskCreate(lcd_task, "lcd_task", 4096, NULL, 1, NULL);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}