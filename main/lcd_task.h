#ifndef LCD_TASK_H
#define LCD_TASK_H

#include "freertos/FreeRTOS.h"

void lcd_i2c_init(void);
void lcd_task(void *pv);

#endif /* LCD_TASK_H */
