#ifndef SERVICE_NVS_SAVE_DATA_SENSOR_H
#define SERVICE_NVS_SAVE_DATA_SENSOR_H

#include "../service_sensor_distance_led/struct/struct_sensor_distance_data.h"
#include "../service_sensor_distance_led/service_sensor_distance.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class service_nvs_save_data_sensor
{
    private:
            /* data */
    public:
        static esp_err_t saveSensorData(sensor_distance_data_t *data, size_t numSensors, struct_color_t *color);
        static esp_err_t loadSensorData(sensor_distance_data_t *data, ServiceSensorDistance *serviceDistance, struct_color_t *color);
        
};
    
#endif