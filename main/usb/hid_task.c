//
// Created by Wind on 2024/12/11.
//

#include <stdlib.h>
#include <esp_task_wdt.h>
#include <lwip/udp.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "event_groups.h"
#include "esp_log.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "FreeRTOSConfig.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "tusb_config.h"
#include "tusb.h"
#include "DAP.h"
#include "wifi.h"

//#include "cdc_task.h"
//#include "DAP.h"
//#include "SWD_flash.h"
//#include "SWD_host.h"
static const char *TAG = "HID_TASK";

uint8_t MYUSB_Request[64 + 1];    // Request  Buffer
uint8_t MYUSB_Response[64 + 1]; // Response Buffer
extern uint8_t WINUSB_Request[64];    // Request  Buffer
extern uint8_t WINUSB_Response[64]; // Response Buffer
uint8_t dealing_data = 0;
int hid_len;
extern bool connect_success;
extern int connect_socket;
TaskHandle_t hid_task_handle = {0};

void hid_task(void *params) {
    (void) params;
    while (1) {

        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
//                DAP_ProcessCommand(MYUSB_Request, MYUSB_Response);
//                tud_hid_report(0, MYUSB_Response, 64);
//                dealing_data = 0;
            ESP_LOGI(TAG, "GetData");

            struct pbuf *ptr = pbuf_alloc(PBUF_TRANSPORT, sizeof(MYUSB_Request), PBUF_POOL);
            pbuf_take(ptr, MYUSB_Request, sizeof(MYUSB_Request));
            udp_send(pcb, ptr);
            pbuf_free(ptr);
            ESP_LOGI(TAG, "GetDat1");
            dealing_data = 0;
        }
//         if (tud_vendor_available())
//         {
//         	uint32_t count = tud_vendor_read(WINUSB_Request, 64);
//         	DAP_ProcessCommand(WINUSB_Request, WINUSB_Response);
//         	tud_vendor_write(WINUSB_Response, 63);
//         }
    }
}

void dap_hid_init() {
    xTaskCreate(hid_task, "hid", 4096, NULL, 1, &hid_task_handle);
}

#if CFG_TUD_HID

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
    // TODO not Implemented
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
    (void) report_id;
    (void) report_type;
    if (buffer[0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1;
        return;
    }
    //没有在处理数据过程中才会接收 不然直接退出
    if (dealing_data)
        return; // Discard packet when buffer is full
    memcpy(MYUSB_Request, buffer, bufsize);
    dealing_data = 1;
    ESP_LOGI("HID","get report");
    xTaskNotifyGive(hid_task_handle);
    // echo back anything we received from host
    // tud_hid_report(0, buffer, bufsize);
}

#endif
