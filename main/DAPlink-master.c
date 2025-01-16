#include <stdio.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include "lcd.h"
#include "gui.h"
#include "wifi.h"
#include "DAP_config.h"
#include "usb.h"
#include "vfs.h"


void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
//    esp_vfs_spiffs_conf_t conf = {
//            .base_path = "/fonts",
//            .partition_label = "fonts",
//            .max_files = 5,
//    };
//
//    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
    DAP_SETUP();
    lcd_init();
    gui_init();
    wifi_init();
    vfs_init();
    usb_init();
}
