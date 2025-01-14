//
// Created by Wind on 2024/12/15.
//

#ifndef SMART_DAPLINK_VFS_INTERFACE_H
#define SMART_DAPLINK_VFS_INTERFACE_H

#include <esp_vfs_fat.h>

esp_err_t init_fat(sdmmc_card_t **card_handle, const char *base_path);

#endif //SMART_DAPLINK_VFS_INTERFACE_H
