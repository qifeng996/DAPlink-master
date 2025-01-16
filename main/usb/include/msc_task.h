//
// Created by Wind on 2024/12/11.
//

#ifndef ESP32DAPLINK_MSC_TASK_H
#define ESP32DAPLINK_MSC_TASK_H

#include "tusb_config.h"

extern bool ejected[LOGICAL_DISK_NUM];
void msc_task(void *params);
#endif //ESP32DAPLINK_MSC_TASK_H
