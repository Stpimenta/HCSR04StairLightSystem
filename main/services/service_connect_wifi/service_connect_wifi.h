#ifndef SERVICE_CONNECT_WIFI
#define SERVICE_CONNECT_WIFI

#include "esp_err.h"
#include "esp_event.h"

class service_connect_wifi
{
private:
    // Handler de eventos Wi-Fi e IP
    static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

public:

    // Método para conectar ao Wi-Fi
    static void wifi_connection(const char* ssid, const char* password, const char* device_name);

    //desconect wifi
    static void wifi_desconnect();

    //reconect wifi
    static void wifi_reconnect();

    // Método para inicializar o NVS
    static void initialize_nvs();
};

#endif // SERVICE_CONNECT_WIFI
