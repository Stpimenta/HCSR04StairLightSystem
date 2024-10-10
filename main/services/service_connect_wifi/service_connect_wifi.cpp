#include "service_connect_wifi.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/ip_addr.h"
#include "nvs_flash.h"
#include <cstring> // Para strcpy e strncpy

static const char *TAG = "service_connect_wifi";



void service_connect_wifi::wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            printf("WIFI CONNECTING....\n");
        } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
            printf("WiFi CONNECTED\n");
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            printf("WiFi lost connection\n");
            printf("Retrying to Connect...\n");
            while (true) {
                esp_wifi_connect();
                vTaskDelay(pdMS_TO_TICKS(5000)); // Delay de 5 segundos entre tentativas
            }
        }
    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            printf("WiFi got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        }
    }
}

void service_connect_wifi::wifi_connection(const char* ssid, const char* password, const char* device_name) {
    // 1 - Wi-Fi Initialization
    esp_netif_init();
    esp_event_loop_create_default(); // Event loop
    esp_netif_create_default_wifi_sta(); // WiFi station

    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif) {
        esp_netif_set_hostname(netif, device_name);
    }

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration = {};
    strncpy((char *)wifi_configuration.sta.ssid, ssid, sizeof(wifi_configuration.sta.ssid) - 1);
    strncpy((char *)wifi_configuration.sta.password, password, sizeof(wifi_configuration.sta.password) - 1);

    esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);
    
    // 2 - Wi-Fi Start
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    // 3 - Wi-Fi Connect
    esp_wifi_connect();
    ESP_LOGI(TAG, "wifi_connection finished. SSID:%s  password:%s  device_name:%s", ssid, password, device_name);
}

void service_connect_wifi::wifi_desconnect() {
    esp_err_t ret = esp_wifi_stop();
    if (ret == ESP_OK) {
        printf("Disconnected Wifi \n");
    } else {
        printf("Failed to disconnect Wifi: %d\n", ret);
    }
}

void service_connect_wifi::wifi_reconnect() {
    esp_err_t ret = esp_wifi_start();
    if (ret == ESP_OK) {
        printf("Reconnect Wifi \n");
    } else {
        printf("Failed to reconnect Wifi: %d\n", ret);
    }
}

void service_connect_wifi::initialize_nvs() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}
