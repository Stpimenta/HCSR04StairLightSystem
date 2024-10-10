#ifndef SERVICE_HTTP_SERVER_SENSOR_DISTANCE_H
#define SERVICE_HTTP_SERVER_SENSOR_DISTANCE_H
#include "esp_http_server.h"
#include <esp_log.h>
#include "services/service_sensor_distance_led/struct/struct_sensor_distance_data.h"
#include "services/service_sensor_distance_led/service_sensor_distance.h"
#include "services/service_calibrate_distance_sensor/service_calibrate_distance_sensor.h"
#include <stdio.h>  // Necessário para snprintf
#include <string.h> // Necessário para strlen
#include "cJSON.h"
#include "struct/struct_task.h"
#include "./html_page/index_html.h"

    class service_http_server_sensor_distance
    {

        private:
            static esp_err_t getDistance(httpd_req_t *req);
            static esp_err_t getSensors(httpd_req_t *req);
            static esp_err_t putDistanceSensor(httpd_req_t *req);
            static esp_err_t putRangeStrip(httpd_req_t *req);
            static esp_err_t putColorStrip(httpd_req_t *req);
            static esp_err_t optionsHandler(httpd_req_t *req);
            static esp_err_t getHtmlPage(httpd_req_t *req);
            static esp_err_t saveConfig(httpd_req_t *req);
            static esp_err_t loadConfig(httpd_req_t *req);

        public:
            static httpd_handle_t start_webserver(sensor_distance_data_t *sensors, size_t numSensor, struct_color_t *color, ServiceSensorDistance *serviceDistance);
            // static void createTaskWebServer(void *pvParameters);
            static void stopWebserver();
       
    };


#endif 
