#ifndef LCD_TASK_H
#define LCD_TASK_H

#include "freertos/FreeRTOS.h"

void lcd_task_start(QueueHandle_t q);
void lcd_i2c_init(void);

#endif /* LCD_TASK_H */
