//
// Created by Wind on 2025/1/16.
//

#ifndef DAPLINK_MASTER_VFS_CONFIG_H
#define DAPLINK_MASTER_VFS_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif
#define USE_SDMMC 0

#if USE_SDMMC
#define SDMMC_CLK GPIO_NUM_15
#define SDMMC_CMD GPIO_NUM_16
#define SDMMC_D0 GPIO_NUM_17
#define SDMMC_D1 GPIO_NUM_18
#define SDMMC_D2 GPIO_NUM_13
#define SDMMC_D3 GPIO_NUM_14
#endif
#ifdef __cplusplus
}
#endif

#endif //DAPLINK_MASTER_VFS_CONFIG_H
