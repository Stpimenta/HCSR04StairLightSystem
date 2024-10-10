#ifndef STRUCT_SENSOR_DISTANCE_DATA_H
#define STRUCT_SENSOR_DISTANCE_DATA_H

#include <driver/gpio.h>
#include <ultrasonic.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


struct sensor_distance_data_t {
    ultrasonic_sensor_t sensor;
    uint32_t distance;
    bool state_1;
    bool state_2;
    TimerHandle_t timer_1;
    TimerHandle_t timer_2;
    uint16_t actionDistanceMin_1;
    uint16_t actionDistanceMax_1;
    uint16_t actionDistanceMin_2;
    uint16_t actionDistanceMax_2;
    uint32_t startLed_1;
    uint32_t endLed_1;
    uint32_t startLed_2;
    uint32_t endLed_2;
};

#endif // STRUCT_SENSOR_DISTANCE_DATA_H
