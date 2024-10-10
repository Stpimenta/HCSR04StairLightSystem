#ifndef SERVICE_SENSOR_DISTANCE_H
#define SERVICE_SENSOR_DISTANCE_H

#include "struct/struct_sensor_distance_data.h"
#include "hardware/strip_controller/stripcontroller.h"
#include "struct/struct_color.h"


extern "C" {
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include <freertos/queue.h>
    #include <ultrasonic.h>
}

class ServiceSensorDistance{
    public:

        //construtor
        ServiceSensorDistance(sensor_distance_data_t *config, size_t numSensors,stripController *ledStrip, struct_color_t* color);

        //servivo para startar as tasks
        void startTasks();

        //handle para monitorar
        TaskHandle_t  sensorTaskHandler = NULL;
        TaskHandle_t  controlTaskHandler = NULL; 

    private:

    //ponteiro para os sensores
    sensor_distance_data_t* config;
    size_t numSensors;

  
    //tasks internas
    static void sensorTask(void* pvParameters);
    static void controlTask(void* pvParameters);

    //semaphoro para leitura e escrita nos sensores
    SemaphoreHandle_t sensorSemaphore;
    
    //strip
    stripController *ledStrip;

    //semaphoro para resgurdar a fita
    SemaphoreHandle_t stripSemaphore;

    //color
    struct_color_t* color;

};


#endif