#ifndef HARDWARE_H
#define HARDWARE_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t lcd_queue;

void lcd_i2c_init(void);
void pir_gpio_init(void);
void pir_task(void *pv);
void lcd_task(void *pv);

#endif /* HARDWARE_H */
