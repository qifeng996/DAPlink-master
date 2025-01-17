//
// Created by Wind on 2025/1/16.
//

#include <esp_vfs_fat.h>
#include <driver/sdmmc_host.h>
#include "vfs.h"
#include "vfs_config.h"


void vfs_init(){
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = true,
            .max_files = 20,
            .allocation_unit_size = 16 * 1024
    };
    const char mount_point[] = "/disk";


#if USE_SDMMC
    sdmmc_card_t *card = NULL;

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
    slot_config.clk = SDMMC_CLK;
    slot_config.cmd = SDMMC_CMD;
    slot_config.d0 = SDMMC_D0;
    slot_config.d1 = SDMMC_D1;
    slot_config.d2 = SDMMC_D2;
    slot_config.d3 = SDMMC_D3;

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI("fat", "Mounting filesystem");
    esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
#else
    wl_handle_t wl_handle_1 = WL_INVALID_HANDLE;
    esp_vfs_fat_spiflash_mount_rw_wl(mount_point, "storage", &mount_config, &wl_handle_1);
#endif
}