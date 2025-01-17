//
// Created by Wind on 2025/1/14.
//

#include <driver/i2c.h>
#include <esp_log.h>
#include "gui.h"
#include "esp_lvgl_port.h"
#include "lcd.h"
#include "esp_lcd_touch_ft5x06.h"
#include "widgets/lv_demo_widgets.h"
#include "lcd_config.h"
#include "esp_wifi.h"
typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;
static lv_indev_t *lvgl_touch_indev = NULL;
static lv_disp_t * disp_handle;
static disp_size_t disp_size;
static const lv_font_t * font_large;
static const lv_font_t * font_normal;
static lv_obj_t * tv;
static lv_obj_t * calendar;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;
static lv_style_t font_qingniao;
lv_obj_t * list;
TaskHandle_t gui_task_handle = {0};



void clicked_wifi_item_cb(lv_event_t * e){
    if (lv_event_get_code(e)==LV_EVENT_CLICKED){
        if (lvgl_port_lock(0)) {
            LV_IMG_DECLARE(connect)
            LV_IMG_DECLARE(disconnect)
            lv_obj_t *obj = lv_event_get_target_obj(e);
            wifi_info *info = lv_obj_get_user_data(obj);
            if (info->connect){
                info->connect = false;
                lv_image_set_src(lv_obj_get_child(obj,0), &disconnect);
            }else{
                lv_obj_t *parent = lv_obj_get_parent(obj);
                uint32_t count = lv_obj_get_child_count(parent);
                for (int i = 0; i < count; ++i) {
                    lv_obj_t *listItem = lv_obj_get_child(parent,i);
                    wifi_info *wifiInfo = lv_obj_get_user_data(listItem);
                    wifiInfo->connect = false;
                    lv_image_set_src(lv_obj_get_child(listItem,0), &disconnect);
                }
                info->connect = true;

                lv_image_set_src(lv_obj_get_child(obj,0), &connect);

            }
            lvgl_port_unlock();
        }
    }
}
lv_obj_t * create_wifi_item(lv_obj_t * parent, const void * img_src, const char * name, const char * category)
{
    static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
    wifi_info wifiInfo = {0};
    lv_obj_set_user_data(cont,&wifiInfo);
    lv_obj_add_event_cb(cont,clicked_wifi_item_cb,LV_EVENT_CLICKED,NULL);

    lv_obj_t * img = lv_image_create(cont);
    lv_image_set_src(img, img_src);
    lv_obj_set_grid_cell(img, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 2);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text(label, name);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_END, 0, 1);

    label = lv_label_create(cont);
//    lv_obj_add_style(label, &font_qingniao, 0);
    lv_obj_add_style(label, &style_text_muted, 0);
    lv_label_set_text(label, category);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 1, 1);

    return cont;
}
void add_wifi_item_cb(lv_event_t * e){
    if (lv_event_get_code(e)==(lv_event_code_t)ADD_WIFI_ITEM){
        add_wifi_item_config_t *msg = lv_event_get_param(e);
        LV_IMG_DECLARE(disconnect)
        if (lvgl_port_lock(0)) {
            create_wifi_item(lv_event_get_target_obj(e), &disconnect, "SN:", msg->ssid);
            lvgl_port_unlock();
        }

    }
}





void lv_gui_widgets(void)
{
    if(LV_HOR_RES <= 320) disp_size = DISP_SMALL;
    else if(LV_HOR_RES < 720) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    int32_t tab_h;
    if(disp_size == DISP_LARGE) {
        tab_h = 70;
#if LV_FONT_MONTSERRAT_24
        font_large     = &lv_font_montserrat_24;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_24 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_16
        font_normal    = &lv_font_montserrat_16;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_16 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else if(disp_size == DISP_MEDIUM) {
        tab_h = 45;
#if LV_FONT_MONTSERRAT_20
        font_large     = &lv_font_montserrat_20;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_20 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_14
        font_normal    = &lv_font_montserrat_14;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_14 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else {   /* disp_size == DISP_SMALL */
        tab_h = 30;
#if LV_FONT_MONTSERRAT_18
        font_large     = &lv_font_montserrat_18;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_18 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_12
        font_normal    = &lv_font_montserrat_12;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_12 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }

#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);
#endif

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

//    lv_font_t *font = lv_freetype_font_create("/fonts/font.ttf",LV_FREETYPE_FONT_RENDER_MODE_BITMAP,14,LV_FREETYPE_FONT_STYLE_NORMAL);
//
//    lv_style_init(&font_qingniao);
//    lv_style_set_text_font(&font_qingniao,font);
    tv = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tv, tab_h);

    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);
//
    lv_obj_t * t1 = lv_tabview_add_tab(tv, "WIFI");
    lv_obj_t * t2 = lv_tabview_add_tab(tv, "FILE");
    lv_obj_t * t3 = lv_tabview_add_tab(tv, "COM");
    lv_obj_t * t4 = lv_tabview_add_tab(tv, "SETTING");
    lv_obj_set_flex_flow(t1, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_flow(t2, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_flow(t3, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_flow(t4, LV_FLEX_FLOW_ROW_WRAP);
    if(disp_size == DISP_LARGE) {
        lv_obj_t * tab_bar = lv_tabview_get_tab_bar(tv);
        lv_obj_set_style_pad_left(tab_bar, LV_HOR_RES / 2, 0);
        lv_obj_t * logo = lv_image_create(tab_bar);
        lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
        LV_IMAGE_DECLARE(img_lvgl_logo);
        lv_image_set_src(logo, &img_lvgl_logo);
        lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);

        lv_obj_t * label = lv_label_create(tab_bar);
        lv_obj_add_style(label, &style_title, 0);
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_label_set_text_fmt(label, "LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

        label = lv_label_create(tab_bar);
        lv_label_set_text(label, "Widgets demo");
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_add_style(label, &style_text_muted, 0);
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
    }



    list = lv_obj_create(t1);
    if(disp_size == DISP_SMALL) {
        lv_obj_add_flag(list, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_set_height(list, LV_PCT(100));
    }
    else {
        lv_obj_set_height(list, LV_PCT(100));
        lv_obj_set_style_max_height(list, 300, 0);
    }

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(list, 1);
    lv_obj_add_flag(list, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    lv_obj_t *title = lv_label_create(list);
    lv_label_set_text(title, "Scanning.....");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_event_cb(list,add_wifi_item_cb,ADD_WIFI_ITEM,NULL);
    lv_obj_t *file = lv_obj_create(t2);
    lv_obj_add_flag(file, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_set_height(file, LV_PCT(100));
    lv_obj_set_flex_flow(file, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(file, 1);
    lv_obj_add_flag(file, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_t *com = lv_obj_create(t3);
    lv_obj_add_flag(com, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_set_height(com, LV_PCT(100));
    lv_obj_set_flex_flow(com, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(com, 1);
    lv_obj_add_flag(com, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_t *setting = lv_obj_create(t4);
    lv_obj_add_flag(setting, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_set_height(setting, LV_PCT(100));
    lv_obj_set_flex_flow(setting, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(setting, 1);
    lv_obj_add_flag(setting, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
//    color_changer_create(tv);
}
void gui_init(){
    lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_cfg.task_stack = 10240;
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
            .io_handle = io_handle,
            .panel_handle = panel_handle,
            .buffer_size = LCD_WIDTH * LCD_HEIGTH,
            .double_buffer = true,
            .hres = LCD_HEIGTH,
            .vres = LCD_WIDTH,
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
            .x_max = LCD_WIDTH,
            .y_max = LCD_HEIGTH,
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
        extern void welcome_ui();
        welcome_ui();
        lvgl_port_unlock();
    }
    vTaskDelay(pdMS_TO_TICKS(4000));
//    lv_freetype_init(2);
    if (lvgl_port_lock(0)) {
        lv_gui_widgets();
        lvgl_port_unlock();
    }
}