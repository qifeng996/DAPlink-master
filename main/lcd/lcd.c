//
// Created by Wind on 2025/1/14.
//

#include <esp_lcd_io_i80.h>
#include "lcd.h"
#include "lcd_config.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_st7789.h"
#include <esp_lcd_panel_ops.h>
esp_lcd_panel_io_handle_t io_handle = NULL;
esp_lcd_panel_handle_t panel_handle = NULL;
void lcd_init(){
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
            .clk_src = LCD_CLK_SRC_DEFAULT,
            .dc_gpio_num = LCD_DC,
            .wr_gpio_num = LCD_WR,
            .data_gpio_nums = {
                    LCD_D0,
                    LCD_D1,
                    LCD_D2,
                    LCD_D3,
                    LCD_D4,
                    LCD_D5,
                    LCD_D6,
                    LCD_D7,
#if LCD_BUS_16
                    LCD_D8,
                    LCD_D9,
                    LCD_D10,
                    LCD_D11,
                    LCD_D12,
                    LCD_D13,
                    LCD_D14,
                    LCD_D15,
#endif
            },
#if LCD_BUS_16
            .bus_width = 16,
#else
            .bus_width = 8,
#endif
            .max_transfer_bytes = LCD_WIDTH * LCD_HEIGTH * sizeof(uint16_t),
            .dma_burst_size = 64,

    };

    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_i80_config_t io_config = {
            .cs_gpio_num = -1,
            .pclk_hz = 2000000,
            .trans_queue_depth = 10,
            .dc_levels = {
                    .dc_idle_level = 0,
                    .dc_cmd_level = 0,
                    .dc_dummy_level = 0,
                    .dc_data_level = 1,
            },
            .flags = {
                    .swap_color_bytes = 0,
            },
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,

    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = -1,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .bits_per_pixel = 16,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

// 初始化屏幕
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    esp_lcd_panel_invert_color(panel_handle, true);

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
}