//
// Created by Wind on 2024/12/12.
//

#include <stdlib.h>
#include <esp_task_wdt.h>
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
#include "driver/uart.h"

#define UART_NUM          UART_NUM_0
#define UART_BAUD_RATE    115200
static const char *TAG = "CDC_TASK";
TaskHandle_t cdc_task_handle;
QueueHandle_t uart_queue;

void cdc_rx_task(void *params) {
    (void) params;

    // RTOS forever loop
    while (1) {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
#if CFG_TUD_CDC
            if (tud_cdc_connected()) {
                uint8_t buf[64];
                // read and echo back
                uint32_t count = tud_cdc_read(buf, sizeof(buf));
                uart_write_bytes(UART_NUM, buf, count);
            }
#endif
        }
    }
}
#if CFG_TUD_CDC
void cdc_tx_task(void *params) {
    (void) params;
    uart_event_t event;
    uint8_t rx_buffer[512];
    while (1) {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY)) {
            int len;
            switch (event.type) {
                case UART_DATA:
                    len = uart_read_bytes(UART_NUM, rx_buffer, event.size, portMAX_DELAY);
                    for (int i = 0; i < len; ++i) {
                        tud_cdc_write_char((char) rx_buffer[i]);
                    }
                    tud_cdc_write_flush();
                    break;
                case UART_BREAK:
                    break;
                case UART_BUFFER_FULL:
                    break;
                case UART_FIFO_OVF:
                    uart_flush_input(UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                case UART_FRAME_ERR:
                    break;
                case UART_PARITY_ERR:
                    break;
                case UART_DATA_BREAK:
                    break;
                case UART_PATTERN_DET:
                    break;
                case UART_WAKEUP:
                    break;
                case UART_EVENT_MAX:
                    break;
            }
        }
    }
}

void dap_cdc_init() {
    uart_config_t uart_config = {
            .baud_rate = UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_queue = xQueueCreate(2, sizeof(uart_event_t));
    uart_param_config(UART_NUM, &uart_config);
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, 512, 512, 2, &uart_queue, ESP_INTR_FLAG_LEVEL3));
    xTaskCreate(cdc_rx_task, "cdc_rx", 4096, NULL, 3, &cdc_task_handle);
    xTaskCreate(cdc_tx_task, "cdc_tx", 4096, NULL, 3, NULL);

}



void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    if (dtr && rts) {
        tud_cdc_write_str("Welcome to tinyUSB CDC example!!!\r\n");
    }
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const *p_line_coding) {
    uart_set_baudrate(UART_NUM, p_line_coding->bit_rate);
}

void tud_cdc_rx_cb(uint8_t itf) {
    if (tud_cdc_available()) {
        xTaskNotifyGive(cdc_task_handle);
    }
}

#endif