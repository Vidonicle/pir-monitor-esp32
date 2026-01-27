#include "hardware.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "events.h"
#include "hal/gpio_types.h"
#include "lcd1602.h"

#define PIR_PIN GPIO_NUM_26

#define LCD_PORT I2C_NUM_0
#define SDA_PIN 21
#define SCL_PIN 22

#define DEBOUNCE_HIGH 500  // 500ms of stable high
#define DEBOUNCE_LOW 500   // 500Ms of no motion to clear

QueueHandle_t lcd_queue;

void lcd_i2c_init(void) {
    i2c_config_t cfg = {.mode = I2C_MODE_MASTER,
                        .sda_io_num = SDA_PIN,
                        .scl_io_num = SCL_PIN,
                        .sda_pullup_en = GPIO_PULLUP_ENABLE,
                        .scl_pullup_en = GPIO_PULLUP_ENABLE,
                        .master.clk_speed = 100000};

    i2c_param_config(LCD_PORT, &cfg);
    i2c_driver_install(LCD_PORT, cfg.mode, 0, 0, 0);
}

void pir_gpio_init(void) {
    gpio_config_t cfg = {.pin_bit_mask = 1ULL << PIR_PIN,
                         .mode = GPIO_MODE_INPUT,
                         .pull_up_en = GPIO_PULLUP_DISABLE,
                         .pull_down_en = GPIO_PULLDOWN_ENABLE,
                         .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&cfg);
}

void pir_task(void *pv) {
    const TickType_t sample_period = pdMS_TO_TICKS(20);
    const TickType_t debounce_rise = pdMS_TO_TICKS(DEBOUNCE_HIGH);
    const TickType_t debounce_fall = pdMS_TO_TICKS(DEBOUNCE_LOW);

    int last_level = 0;
    int state = 0;  // 0 = idle, 1 = motion
    TickType_t last_change = xTaskGetTickCount();

    for (;;) {
        int level = gpio_get_level(PIR_PIN);

        if (level != last_level) {
            last_change = xTaskGetTickCount();
            last_level = level;
        }

        TickType_t elapsed = xTaskGetTickCount() - last_change;

        if (!state && level && elapsed > debounce_rise) {
            pir_event_t e = PIR_EVENT_MOTION_START;
            xQueueSend(lcd_queue, &e, 0);
            ESP_LOGI("PIR", "Motion detected");
            state = 1;
        }

        if (state && !level && elapsed > debounce_fall) {
            pir_event_t e = PIR_EVENT_MOTION_STOP;
            xQueueSend(lcd_queue, &e, 0);
            ESP_LOGI("PIR", "Motion stopped");
            state = 0;
        }

        vTaskDelay(sample_period);
    }
}

void lcd_task(void *pv) {
    pir_event_t e;
    lcd_write("PIR Monitor", "Waiting...");
    while (1) {
        if (xQueueReceive(lcd_queue, &e, portMAX_DELAY)) {
            if (e == PIR_EVENT_MOTION_START) {
                lcd_write("PIR: ALERT", "MOTION DETECTED");
            } else if (e == PIR_EVENT_MOTION_STOP) {
                lcd_write("PIR: IDLE", "STANDBY");
            }
        }
    }
}