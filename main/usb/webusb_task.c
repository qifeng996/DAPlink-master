//
// Created by Wind on 2024/12/12.
//

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "tinyusb.h"
#include "webusb_task.h"
#include "DAP.h"

static const char *TAG = "WEB_TASK";

enum
{
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};

#define URL "CMSIS-DAP v2"

#if CFG_TUD_VENDOR

#define BOS_TOTAL_LEN (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

#define MS_OS_20_DESC_LEN 0xB2

// BOS Descriptor is required for webUSB
uint8_t const desc_bos[] =
        {
                // total length, number of device caps
                TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

                // Vendor Code, iLandingPage
                TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),

                // Microsoft OS 2.0 descriptor
                TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT)};

uint8_t const *tud_descriptor_bos_cb(void)
{
    return desc_bos;
}

uint8_t const desc_ms_os_20[] =
        {
                // Set header: length, type, windows version, total length
                U16_TO_U8S_LE(0x000A), U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR), U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

                // Configuration subset header: length, type, configuration index, reserved, configuration total length
                U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION), 0, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A),

                // Function Subset header: length, type, first interface, reserved, subset length
                U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION), 3, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08),

                // MS OS 2.0 Compatible ID descriptor: length, type, compatible ID, sub compatible ID
                U16_TO_U8S_LE(0x0014), U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID), 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sub-compatible

                // MS OS 2.0 Registry property descriptor: length, type
                U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08 - 0x08 - 0x14), U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
                U16_TO_U8S_LE(0x0007), U16_TO_U8S_LE(0x002A), // wPropertyDataType, wPropertyNameLength and PropertyName "DeviceInterfaceGUIDs\0" in UTF-16
                'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
                'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00,
                U16_TO_U8S_LE(0x0050), // wPropertyDataLength
                //"{CDB3B5AD-293B-4663-AA36-1AAE46463776}"
                //bPropertyData: “{975F44D9-0D08-43FD-8B3E-127CA8AFFF9D}”.
                '{', 0x00, 'C', 0x00, 'D', 0x00, 'B', 0x00, '3', 0x00, 'B', 0x00, '5', 0x00, 'A', 0x00, 'D', 0x00, '-', 0x00,
                '2', 0x00, '9', 0x00, '3', 0x00, 'B', 0x00, '-', 0x00, '4', 0x00, '6', 0x00, '6', 0x00, '3', 0x00, '-', 0x00,
                'A', 0x00, 'A', 0x00, '3', 0x00, '6', 0x00, '-', 0x00, '1', 0x00, 'A', 0x00, 'A', 0x00, 'E', 0x00, '4', 0x00,
                '6', 0x00, '4', 0x00, '6', 0x00, '3', 0x00, '7', 0x00, '7', 0x00, '6', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00};

TU_VERIFY_STATIC(sizeof(desc_ms_os_20) == MS_OS_20_DESC_LEN, "Incorrect size");

const tusb_desc_webusb_url_t desc_url =
        {
                .bLength = 3 + sizeof(URL) - 1,
                .bDescriptorType = 3, // WEBUSB URL type
                .bScheme = 1,		  // 0: http, 1: https
                .url = URL};

static bool web_serial_connected = false;

//------------- prototypes -------------//

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// send characters to both CDC and WebUSB


//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+

// Invoked when received VENDOR control request
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    ESP_LOGI(TAG, "I AM RUN");
    switch (request->bRequest)
    {
        case VENDOR_REQUEST_WEBUSB:
            // match vendor request in BOS descriptor
            // Get landing page url
            return tud_control_xfer(rhport, request, (void *)&desc_url, desc_url.bLength);

        case VENDOR_REQUEST_MICROSOFT:
            if (request->wIndex == 7)
            {
                // Get Microsoft OS 2.0 compatible descriptor
                uint16_t total_len;
                memcpy(&total_len, desc_ms_os_20 + 8, 2);

                return tud_control_xfer(rhport, request, (void *)desc_ms_os_20, total_len);
            }
            else
            {
                return false;
            }

        case 0x22:
            // Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
            // connect and disconnect.
            web_serial_connected = (request->wValue != 0);

            // Always lit LED if connected
            if (web_serial_connected)
            {
                tud_vendor_write_str("\r\nTinyUSB WebUSB device example\r\n");
            }

            // response with status OK
            return tud_control_status(rhport, request);

        default:
            // stall unknown request

            // return tud_control_xfer(rhport, request, (void *)&desc_url, desc_url.bLength);
            return false;
    }

    return true;
}

// Invoked when DATA Stage of VENDOR's request is complete
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const *request)
{
    (void)rhport;
    (void)request;

    // nothing to do
    return true;
}
#endif
uint8_t WINUSB_Request[64] = {0};  // Request  Buffer
uint8_t WINUSB_Response[64] = {0}; // Response Buffer
uint8_t WINUSB_data = 0;
int WINUSB_len;
void webusb_task(void *p)
{
    uint32_t time;
    while (1)
    {
#if CFG_TUD_VENDOR

        // if (tud_vendor_available())
        // {
        // 	time = xTaskGetTickCount();
        // 	uint32_t count = tud_vendor_read(WINUSB_Request, 64);
        // 	DAP_ProcessCommand(WINUSB_Request, WINUSB_Response);
        // 	tud_vendor_write(WINUSB_Response, 63);
        // 	ESP_LOGI(TAG, "%d %d", count, xTaskGetTickCount() - time);
        // }
#endif
        vTaskDelay(pdMS_TO_TICKS(10));

    }
}
