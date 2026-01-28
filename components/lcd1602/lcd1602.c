#include "lcd1602.h"

#include <stdint.h>

#include "driver/i2c.h"
#include "esp_rom_sys.h"

#define PIN_RS 0
#define PIN_EN 2
#define PIN_BL 3
#define LCD_CMD 0
#define LCD_DATA 1

#define COLUMNS 16
#define ROWS 2

static i2c_port_t s_port;
static uint8_t s_addr;
static uint8_t s_backlight;

static void pcf_write(uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (s_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(s_port, cmd, pdMS_TO_TICKS(20));
    i2c_cmd_link_delete(cmd);
}

static void lcd_pulse(uint8_t data) {
    pcf_write(data | (1 << PIN_EN));
    esp_rom_delay_us(50);
    pcf_write(data & ~(1 << PIN_EN));
    esp_rom_delay_us(50);
}

static void lcd_write4(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << 4);
    if (rs)
        data |= (1 << PIN_RS);
    if (s_backlight)
        data |= (1 << PIN_BL);
    lcd_pulse(data);
}

static void lcd_write8(uint8_t data, uint8_t rs) {
    lcd_write4(data >> 4, rs);
    lcd_write4(data & 0x0F, rs);
}

static void lcd_cmd(uint8_t cmd) {
    lcd_write8(cmd, LCD_CMD);
    esp_rom_delay_us(50);
}

static void lcd_data(uint8_t data) {
    lcd_write8(data, LCD_DATA);
    esp_rom_delay_us(50);
}

void lcd_init(i2c_port_t port, uint8_t addr, bool backlight) {
    s_port = port;
    s_addr = addr;
    s_backlight = backlight ? 1 : 0;

    esp_rom_delay_us(50000);

    lcd_write4(0x3, 0);
    esp_rom_delay_us(5000);
    lcd_write4(0x3, 0);
    esp_rom_delay_us(5000);
    lcd_write4(0x3, 0);
    esp_rom_delay_us(5000);
    lcd_write4(0x2, 0);

    lcd_cmd(0x28);  // 4-bit, 2-line
    lcd_cmd(0x0C);  // display on
    lcd_cmd(0x06);  // entry mode
    lcd_cmd(0x01);  // clear
    esp_rom_delay_us(2000);
}

void lcd1602_clear(void) {
    lcd_cmd(0x01);
    esp_rom_delay_us(2000);
}

void lcd_write_line(uint8_t row, const char *text) {
    lcd_cmd(row == 0 ? 0x80 : 0xC0);

    for (uint8_t i = 0; i < COLUMNS && text[i]; i++) {
        lcd_data(text[i]);
    }
}

void lcd_write(const char *line0, const char *line1) {
    lcd1602_clear();

    lcd_write_line(0, line0 ? line0 : "");
    lcd_write_line(1, line1 ? line1 : "");
}