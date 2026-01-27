#ifndef LCD1602_H
#define LCD1602_H

#include <stdint.h>

#include "driver/i2c.h"

void lcd_init(i2c_port_t port, uint8_t addr, bool backlight);
void lcd_write_line(uint8_t row, const char *text);
void lcd_write(const char *line0, const char *line1);

#endif
