//
// Created by Wind on 2025/1/14.
//

#ifndef DAPLINK_MASTER_LCD_H
#define DAPLINK_MASTER_LCD_H
#include "esp_lcd_panel_st7789.h"
#include <esp_lcd_panel_ops.h>
extern esp_lcd_panel_io_handle_t io_handle;
extern esp_lcd_panel_handle_t panel_handle;

void lcd_init();
#endif //DAPLINK_MASTER_LCD_H
