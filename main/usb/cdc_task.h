//
// Created by Wind on 2024/12/12.
//

#ifndef ESP32DAPLINK_CDC_TASK_H
#define ESP32DAPLINK_CDC_TASK_H
extern TaskHandle_t cdc_task_handle;

void cdc_task(void *params);

void dap_cdc_init();

#endif //ESP32DAPLINK_CDC_TASK_H
