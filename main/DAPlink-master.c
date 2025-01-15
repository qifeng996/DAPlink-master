#include <stdio.h>
#include <esp_private/usb_phy.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include "lcd.h"
#include "gui.h"
#include "device/usbd.h"
#include "wifi.h"
static void usb_phy_init(void) {
    usb_phy_handle_t phy_hdl;
    // Configure USB PHY
    usb_phy_config_t phy_conf = {
            .controller = USB_PHY_CTRL_OTG,
            .otg_mode = USB_OTG_MODE_DEVICE,
            .target = USB_PHY_TARGET_INT,
    };
    usb_new_phy(&phy_conf, &phy_hdl);
}

static void tusb_device_task(void *arg) {
    while (1) {
        tud_task();
    }
}

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
    lcd_init();
    gui_init();
    wifi_init();
//    usb_phy_init();
//    tud_init(0);
}
