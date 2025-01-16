//
// Created by Wind on 2024/12/11.
//

#ifndef ESP32DAPLINK_HID_TASK_H
#define ESP32DAPLINK_HID_TASK_H
extern TaskHandle_t hid_task_handle;
extern uint8_t MYUSB_Request[64 + 1];    // Request  Buffer
extern uint8_t MYUSB_Response[64 + 1]; // Response Buffer
extern uint8_t dealing_data;
void hid_task(void *params);

void dap_hid_init();

#endif //ESP32DAPLINK_HID_TASK_H
