#include "pir_task.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "events.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIR_PIN GPIO_NUM_26
#define DEBOUNCE_HIGH 500
#define DEBOUNCE_LOW 500

static QueueHandle_t s_event_queue;

static void pir_gpio_init(void) {
    gpio_config_t cfg = {.pin_bit_mask = 1ULL << PIR_PIN,
                         .mode = GPIO_MODE_INPUT,
                         .pull_up_en = GPIO_PULLUP_DISABLE,
                         .pull_down_en = GPIO_PULLDOWN_ENABLE,
                         .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&cfg);
}

static void pir_task(void *pv) {
    const TickType_t sample_period = pdMS_TO_TICKS(20);
    const TickType_t debounce_rise = pdMS_TO_TICKS(DEBOUNCE_HIGH);
    const TickType_t debounce_fall = pdMS_TO_TICKS(DEBOUNCE_LOW);

    int last_level = 0;
    int state = 0;
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
            xQueueSend(s_event_queue, &e, 0);
            ESP_LOGI("PIR", "Motion detected");
            state = 1;
        }

        if (state && !level && elapsed > debounce_fall) {
            pir_event_t e = PIR_EVENT_MOTION_STOP;
            xQueueSend(s_event_queue, &e, 0);
            ESP_LOGI("PIR", "Motion stopped");
            state = 0;
        }

        vTaskDelay(sample_period);
    }
}

void pir_task_start(QueueHandle_t q) {
    s_event_queue = q;
    pir_gpio_init();
    xTaskCreate(pir_task, "pir_task", 2048, NULL, 1, NULL);
}
