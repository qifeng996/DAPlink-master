//
// Created by Wind on 2025/1/15.
//

#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi_default.h>
#include <esp_wifi.h>
#include <string.h>
#include <esp_log.h>
#include "wifi.h"
#include "lvgl.h"
#include "gui.h"
#include "esp_lvgl_port.h"

const char *TAG = "wifi_task";
TaskHandle_t wifi_task_handle = {0};
void wifi_scan(){
    esp_wifi_scan_start(NULL, true);
    uint16_t ap_count = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    wifi_ap_record_t ap_info[ap_count];
    memset(ap_info, 0, sizeof(ap_info));
    ESP_LOGI(TAG, "Max AP number ap_info can hold = %u", ap_count);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));
    ESP_LOGI(TAG, "Total APs scanned = %u, actual AP number ap_info holds = %u", ap_count, ap_count);
    ESP_LOGI(TAG, "-----------------------------------------------------");
    extern lv_obj_t * list;
    if (lvgl_port_lock(0)) {
        lv_obj_clean(list);
        lvgl_port_unlock();
    }
    for (int i = 0; i < ap_count; i++) {
        if (!strncmp((const char *) ap_info[i].ssid, "DAPLink-", 8)){
            add_wifi_item_config_t msg;
            memcpy(msg.ssid,ap_info[i].ssid+8, 20);
            msg.rssi = (int)ap_info[i].rssi;

            lv_obj_send_event(list,ADD_WIFI_ITEM,&msg);
        }
        vTaskDelay(5);
    }
}
void wifi_task_entry(void *arg){
    vTaskDelay(pdMS_TO_TICKS(2000));
    while(1){
        wifi_scan();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
void wifi_init(){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    xTaskCreate(wifi_task_entry,"wifi_task",15*1024,NULL, 5, &wifi_task_handle);
}



