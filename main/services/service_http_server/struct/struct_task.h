#ifndef STRUCT_TASK_T
#define STRUCT_TASK_T


#include "services/service_sensor_distance_led/struct/struct_sensor_distance_data.h"
#include "services/service_calibrate_distance_sensor/service_calibrate_distance_sensor.h"

    struct struct_task_t
    {
        sensor_distance_data_t *sensors;
        size_t numSensors;
        struct_color_t *color;
    };
    

#endif