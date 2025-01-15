#ifndef DAPLINK_MASTER_GUI_EVENT_H
#define DAPLINK_MASTER_GUI_EVENT_H
typedef struct {
    char ssid[33];
    int rssi;
} add_wifi_item_config_t;
enum CUSTOM_EVENT{
    ADD_WIFI_ITEM = _LV_EVENT_LAST+1,
} ;
#endif //DAPLINK_MASTER_GUI_EVENT_H