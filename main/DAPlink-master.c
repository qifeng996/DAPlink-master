#include <stdio.h>
#include <esp_private/usb_phy.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include <driver/sdmmc_host.h>
#include "lcd.h"
#include "gui.h"
#include "device/usbd.h"
#include "esp_vfs_fat.h"
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

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, eMMC will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = 20,
            .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card = NULL;
    const char mount_point[] = "/path";

    // Use settings defined above to initialize eMMC and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.


    // By default, eMMC frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 52MHz for SDMMC)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_52M;

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Other fields will be initialized to zero
    sdmmc_slot_config_t slot_config = {
            .cd = SDMMC_SLOT_NO_CD,
            .wp = SDMMC_SLOT_NO_WP,
    };

    // Set bus width to use:
    slot_config.width = 4;

    // Set bus IOs
#if CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = GPIO_NUM_15;
    slot_config.cmd = GPIO_NUM_16;
    slot_config.d0 = GPIO_NUM_17;
    slot_config.d1 = GPIO_NUM_18;
    slot_config.d2 = GPIO_NUM_13;
    slot_config.d3 = GPIO_NUM_14;
#endif  // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI("fat", "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
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
