#include <stdio.h>
#include <esp_private/usb_phy.h>
#include "lcd.h"
#include "gui.h"
#include "device/usbd.h"
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
    lcd_init();
    gui_init();
//    usb_phy_init();
//    tud_init(0);
}
