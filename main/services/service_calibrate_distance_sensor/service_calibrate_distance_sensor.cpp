#include <services/service_calibrate_distance_sensor/service_calibrate_distance_sensor.h>


//alter distance sensor 
void service_calibrate_distance_sensor::calibrateDistanceSensor(sensor_distance_data_t* sensor, uint32_t minDistance, uint32_t maxDistance, uint8_t stage)
{
    if(stage == 1)
    {
        sensor->actionDistanceMin_1 = minDistance;
        sensor->actionDistanceMax_1 = maxDistance;
    }

    if(stage == 2)
    {
        sensor->actionDistanceMin_2 = minDistance;
        sensor->actionDistanceMax_2 = maxDistance;
    }
}

//alter distance sensor 
void service_calibrate_distance_sensor::calibrateAroundLed(sensor_distance_data_t* sensor, uint32_t startLed, uint32_t endLed, uint8_t stage)
{
    if(stage == 1)
    {
        sensor->startLed_1 = startLed;
        sensor->endLed_1 = endLed;
    }

    if(stage == 2)
    {
        sensor->startLed_2 = startLed;
        sensor->endLed_2 = endLed;
    }
}

//alter color
void service_calibrate_distance_sensor::changeColor(struct_color_t* color, uint8_t red, uint8_t blue, uint8_t green)
{

    color->red = red;
    color->blue = blue;
    color->green = green;

}