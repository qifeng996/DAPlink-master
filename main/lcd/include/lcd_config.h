#ifndef DAPLINK_MASTER_LCD_CONFIG_H
#define DAPLINK_MASTER_LCD_CONFIG_H
#define LCD_WIDTH 240
#define LCD_HEIGTH 320
#define LCD_BUS_16 0

#define LCD_D0      GPIO_NUM_3
#define LCD_D1      GPIO_NUM_4
#define LCD_D2      GPIO_NUM_5
#define LCD_D3      GPIO_NUM_6
#define LCD_D4      GPIO_NUM_7
#define LCD_D5      GPIO_NUM_8
#define LCD_D6      GPIO_NUM_9
#define LCD_D7      GPIO_NUM_10
#if LCD_BUS_16
#define LCD_D8
#define LCD_D9
#define LCD_D10
#define LCD_D11
#define LCD_D12
#define LCD_D13
#define LCD_D14
#define LCD_D15
#endif
#define LCD_WR      GPIO_NUM_2
#define LCD_DC      GPIO_NUM_1
#define LCD_RST     (-1)
#define TOUCH_SDA   GPIO_NUM_12
#define TOUCH_CLK   GPIO_NUM_11
#endif //DAPLINK_MASTER_LCD_CONFIG_H