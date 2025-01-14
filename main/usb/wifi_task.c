//
// Created by Wind on 2024/12/17.
//

#include <esp_log.h>
#include "wifi_task.h"
#include "esp_wifi.h"
#include "lwip/udp.h"
#include "hid_task.h"
#include "class/hid/hid_device.h"
#include "lwip/ip4_addr.h"
#include "mqtt_client.h"
struct udp_pcb *pcb;
const char TAG[] = "WIFI";
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
        udp_connect(pcb,addr,8090);
        char *data = (char *)p->payload;
        if (dealing_data) {
            pbuf_free(p);
            return;
        }
        ESP_LOGI(TAG, "Loss Connect");

        memcpy(MYUSB_Response,data,64);
        tud_hid_report(0, MYUSB_Response, 64);
        pbuf_free(p);  // 释放接收到的pbuf缓冲区
        dealing_data = 0;
    }
}
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data){
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP:
            ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "Get IP: " IPSTR, IP2STR(&event->ip_info.ip));
            udp_init();
            pcb = udp_new();
            if (udp_bind(pcb, IP_ADDR_ANY, 8090) != ERR_OK) {
                ESP_LOGE(TAG, "UDP bind failed");
                udp_remove(pcb);  // 清理PCB
                return;
            }

            ipaddr_ntoa_r(&remote_addr,"192.168.4.1", strlen("192.168.4.1"));
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
esp_err_t wifi_init(){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
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
    wifi_config_t wifiConfig = {
//            .ap = {
//                    .ssid = "DAPLink_Master",
//                    .ssid_len = strlen("DAPLink_Master"),
//                    .channel = 5,
//                    .password = "password",
//                    .max_connection = 1,
//                    .authmode = WIFI_AUTH_WPA2_PSK,
//                    .pmf_cfg = {
//                            .required = true,
//                    },
//            },
                .sta = {
                        .ssid = "DAPLink_Master",
                        .password = "password",
                }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());
    return ESP_OK;
}
