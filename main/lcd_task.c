#include "lcd_task.h"

#include "driver/i2c.h"
#include "events.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd1602.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define LCD_ADDR 0x27

static QueueHandle_t s_event_queue;

void lcd_i2c_init(void) {
    i2c_config_t cfg = {.mode = I2C_MODE_MASTER,
                        .sda_io_num = SDA_PIN,
                        .scl_io_num = SCL_PIN,
                        .sda_pullup_en = GPIO_PULLUP_ENABLE,
                        .scl_pullup_en = GPIO_PULLUP_ENABLE,
                        .master.clk_speed = 100000};

    i2c_param_config(I2C_NUM_0, &cfg);
    i2c_driver_install(I2C_NUM_0, cfg.mode, 0, 0, 0);
}

static void lcd_task(void *pv) {
    pir_event_t e;

    lcd_write("PIR Monitor", "Waiting...");

    for (;;) {
        if (xQueueReceive(s_event_queue, &e, portMAX_DELAY)) {
            if (e == PIR_EVENT_MOTION_START) {
                lcd_write("PIR: ALERT", "MOTION DETECTED");
            } else if (e == PIR_EVENT_MOTION_STOP) {
                lcd_write("PIR: IDLE", "STANDBY");
            }
        }
    }
}

void lcd_task_start(QueueHandle_t q) {
    s_event_queue = q;

    xTaskCreate(lcd_task, "lcd_task", 4096, NULL, 1, NULL);
}
