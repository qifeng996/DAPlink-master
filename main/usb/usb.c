//
// Created by Wind on 2025/1/16.
//

#include "usb.h"
#include "device/usbd.h"
#include <esp_private/usb_phy.h>
#include "FreeRTOS.h"
#include "cdc_task.h"
#include "hid_task.h"
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
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void usb_init(){
    usb_phy_init();
    tud_init(0);
    xTaskCreate(tusb_device_task, "TinyUSB", 4096, NULL, 5, NULL);

    dap_hid_init();
    dap_cdc_init();
}