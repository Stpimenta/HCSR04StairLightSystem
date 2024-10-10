#ifndef SERVICE_CALIBRATE_DISTANCE_SENSOR
#define SERVICE_CALIBRATE_DISTANCE_SENSOR

#include <services/service_sensor_distance_led/struct/struct_sensor_distance_data.h>
#include <services/service_sensor_distance_led/struct/struct_color.h>

    class service_calibrate_distance_sensor
    {
    private:
        /* data */
    public:
        static void calibrateDistanceSensor(sensor_distance_data_t* sensor, uint32_t minDistance, uint32_t maxDistance, uint8_t stage);
        static void calibrateAroundLed(sensor_distance_data_t* sensor, uint32_t startLed, uint32_t endLed, uint8_t stage);
        static void changeColor(struct_color_t * color, uint8_t red, uint8_t blue, uint8_t green);
    };
    
#endif