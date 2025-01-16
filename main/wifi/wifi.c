//
// Created by Wind on 2025/1/15.
//

#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi_default.h>
#include <esp_wifi.h>
#include <string.h>
#include <esp_log.h>
#include <lwip/udp.h>
#include "wifi.h"
#include "lvgl.h"
#include "gui.h"
#include "esp_lvgl_port.h"
#include "hid_task.h"
#include "class/hid/hid_device.h"

static char *TAG = "wifi_task";
TaskHandle_t wifi_task_handle = {0};
struct udp_pcb *pcb;
ip_addr_t remote_addr;
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "STA Started");
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        ESP_LOGI(TAG, "Loss Connect");
    }
}
void udp_receive_callback(void *arg, struct udp_pcb *_pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p != NULL) {
        char *data = (char *)p->payload;
        ESP_LOGI(TAG, "Loss Connect");
        memcpy(MYUSB_Response,data,64);
        tud_hid_report(0, MYUSB_Response, 64);
        pbuf_free(p);  // 释放接收到的pbuf缓冲区
        dealing_data = 0;
    }
}
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data){
    ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP:

            ESP_LOGI(TAG, "Get IP: " IPSTR, IP2STR(&event->ip_info.ip));
            udp_init();
            pcb = udp_new();
            if (udp_bind(pcb, IP_ADDR_ANY, 8090) != ERR_OK) {
                ESP_LOGE(TAG, "UDP bind failed");
                udp_remove(pcb);  // 清理PCB
                return;
            }

//            ipaddr_ntoa_r(&remote_addr,"192.168.4.1", strlen("192.168.4.1"));
            IP4_ADDR((ip4_addr_t *)&remote_addr, 192, 168, 4, 1);
            udp_connect(pcb,&remote_addr,8090);
            ESP_LOGI(TAG, "UDP receiver started, waiting for data...");

            // 注册接收回调函数
            udp_recv(pcb, udp_receive_callback, NULL);
            break;

        case IP_EVENT_STA_LOST_IP:
            ESP_LOGI(TAG, "IP Loss");
            break;
        default:
            ESP_LOGI(TAG, "No: %ld", event_id);
            break;
    }

}
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
    wifi_scan();
    while(1){
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
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ip_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t wifiConfig = {
            .sta = {
                    .ssid = "DAPLink-sjqh1uwu2im4n",
                    .password = "sjqh1uwu2im4n"
            }
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());

}



