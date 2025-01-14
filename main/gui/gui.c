//
// Created by Wind on 2025/1/14.
//

#include <driver/i2c.h>
#include "gui.h"
#include "esp_lvgl_port.h"
#include "lcd.h"
#include "esp_lcd_touch_ft5x06.h"
#include "widgets/lv_demo_widgets.h"

static lv_indev_t *lvgl_touch_indev = NULL;
static lv_disp_t * disp_handle;
TaskHandle_t gui_task_handle = {0};
void gui_task_entry(void *arg){
    while(1){
        uint32_t task_delay_ms = lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void gui_init(){
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
            .io_handle = io_handle,
            .panel_handle = panel_handle,
            .buffer_size = 240 * 320,
            .double_buffer = true,
            .hres = 320,
            .vres = 240,
            .monochrome = false,
#if LVGL_VERSION_MAJOR >= 9
            .color_format = LV_COLOR_FORMAT_RGB565,
#endif
            .rotation = {
                    .swap_xy = true,
                    .mirror_x = true,
                    .mirror_y = false,
            },
            .flags = {
#if LVGL_VERSION_MAJOR >= 9
                    .swap_bytes = true,
#endif
                    .buff_dma = true,
                    .buff_spiram = true,
                    .sw_rotate = false,
                    .direct_mode= false
            }
    };
    disp_handle = lvgl_port_add_disp(&disp_cfg);

    const i2c_config_t i2c_conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = 11,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = 12,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = 400000
    };
    i2c_param_config(I2C_NUM_0, &i2c_conf);
    i2c_driver_install(I2C_NUM_0, i2c_conf.mode, 0, 0, 0);

    /* Initialize touch HW */
    const esp_lcd_touch_config_t tp_cfg = {
            .x_max = 240,
            .y_max = 320,
            .rst_gpio_num = GPIO_NUM_NC,
            .int_gpio_num = GPIO_NUM_NC,
            .levels = {
                    .reset = 0,
                    .interrupt = 0,
            },
            .flags = {
                    .swap_xy = 1,
                    .mirror_x = 0,
                    .mirror_y = 0,
            },
    };
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
    esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_NUM_0, &tp_io_config, &tp_io_handle);

    esp_lcd_touch_handle_t tp;
    esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, &tp);
    const lvgl_port_touch_cfg_t touch_cfg = {
            .disp = disp_handle,
            .handle = tp,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

    lv_init();

    if (lvgl_port_lock(0)) {
        lv_demo_widgets();
//        example_lvgl_demo_ui(disp_handle);
        lvgl_port_unlock();
    }
    xTaskCreate(gui_task_entry,"gui_task",8192,NULL, 1, &gui_task_handle);
}