#include "service_sensor_distance.h"

 

ServiceSensorDistance::ServiceSensorDistance (sensor_distance_data_t *config, size_t numSensors,stripController *ledStrip, struct_color_t* color) 
    : config(config), numSensors(numSensors), ledStrip(ledStrip), color(color)
{

}




//serviço que vai iniciar as tasks e o semaphoro
void ServiceSensorDistance :: startTasks ()
{
    sensorSemaphore = xSemaphoreCreateMutex();
    stripSemaphore = xSemaphoreCreateMutex();
    
    // Cria a tarefa do sensor e a vincula ao núcleo 0
    xTaskCreatePinnedToCore(sensorTask, "sensorTask", 4096, this, 1, &sensorTaskHandler, 1);

    // Cria a tarefa de controle e a vincula ao núcleo 0
    xTaskCreatePinnedToCore(controlTask, "controlTask", 4096, this, 1, &controlTaskHandler, 1);

  
}


//serviço que vai instanciar o sensor e medir a distancia
void ServiceSensorDistance :: sensorTask(void* pvParameters){

    //faço a cast de pvParams para o compialdor saber o que é
    ServiceSensorDistance* instanceThis = static_cast<ServiceSensorDistance*>(pvParameters);
    if (instanceThis == nullptr || instanceThis->config == nullptr) {
        while (1)
        {
            printf("Invalid parameters in sensorTask\n");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }


    // Inicializa os sensores
    for(int x = 0; x < instanceThis->numSensors; x++){
        //inicio os sensores
        esp_err_t initRes = ultrasonic_init(&instanceThis->config[x].sensor);
        if (initRes != ESP_OK) {
            printf("Erro na inicialização do sensor:%d, erro: %s\n", x, esp_err_to_name(initRes));
        } 
    }


    uint32_t distancecm;

    while (true) {
        //medir os sensores
        for(int i = 0; i < instanceThis->numSensors; i++)
        {
            //receber a distancia
            esp_err_t res = ultrasonic_measure_cm(&instanceThis->config[i].sensor,1000,&distancecm);

            //se retirnar ok adiciono na fila
            if (res == ESP_OK)
            {   
                //semaforo para modificar o ponteiro de distancia
                if(xSemaphoreTake(instanceThis->sensorSemaphore,portMAX_DELAY) == pdTRUE){
                    instanceThis->config[i].distance = distancecm;
                }

                // Libere o semáforo após acessar a variável
                xSemaphoreGive(instanceThis->sensorSemaphore);
            }

            //tratamento de erros
            else
            {   
                switch (res)
                {
                    case ESP_ERR_ULTRASONIC_PING:
                        printf("sensor: %d Erro: Sensor em estado invalido para pulso\n",i);
                        break;
                    case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                        printf("sensor: %d Erro: Invalido\n",i);
                        break;
                    case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                        printf("sensor: %d Erro: Distancia Muito Grande\n",i);
                        break;
                    default:
                        printf("sensor: %d erro: %s\n", i, esp_err_to_name(res));
                }
            }
        }
        //delay para liberar o processador
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


/* funcoes para task consumidora*/

//struct fade task
struct FadeParams
{
    stripController* strip;
    uint32_t startLed;
    uint32_t endLed;
    bool state;
    SemaphoreHandle_t semaphoreStrip;
    TimerHandle_t *timer;
    struct_color_t *color;
};

//create fade task
void fadeTask(void* params)
{
    FadeParams* fadeParams = static_cast<FadeParams*>(params);

    if(xSemaphoreTake(fadeParams->semaphoreStrip, portMAX_DELAY) == pdTRUE)
    {

        if(fadeParams->state)
        {
            fadeParams->strip->onFade(fadeParams->color->red, fadeParams->color->blue, fadeParams->color->green, fadeParams->startLed, fadeParams->endLed, 10);
        }
        else
        {
            fadeParams->strip->offFade(fadeParams->color->red, fadeParams->color->blue, fadeParams->color->green, fadeParams->startLed, fadeParams->endLed, 10);
        }
    }
   xSemaphoreGive(fadeParams->semaphoreStrip);
   delete fadeParams;
   vTaskDelete(NULL);

}

//timer debounceOffCallback

// Função de callback
void myTimerCallback(TimerHandle_t xTimer)
{
    FadeParams* param = (FadeParams*)pvTimerGetTimerID(xTimer);
    *param->timer = NULL;

    // xTaskCreate(fadeTask, "fadeTask", 1024, param, 2, NULL);

    xTaskCreatePinnedToCore(fadeTask, "fadeTask", 2048, param, 2, NULL, 0);
    // printf("desligou timer\n");
}


//task controltask
void ServiceSensorDistance :: controlTask(void* pvParameters)
{
    //faço a cast de pvParams para o compialdor saber o que é
    ServiceSensorDistance* instanceThis = static_cast<ServiceSensorDistance*>(pvParameters);
    if (instanceThis == nullptr || instanceThis->config == nullptr) {
        while (1)
        {
            printf("Invalid parameters in sensorTask\n");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }



    while (1)
    {   
        //verify semaphore
        if(xSemaphoreTake(instanceThis->sensorSemaphore, portMAX_DELAY) == pdTRUE)
        {
            for (int x = 0; x<instanceThis->numSensors; x++)
            {  
             
               //printf("sensor index:%d, distancia:%ld \n",x,instanceThis->config[x].distance);

                //distance action
                if(instanceThis->config[x].distance > instanceThis->config[x].actionDistanceMin_1 && instanceThis->config[x].distance < instanceThis->config[x].actionDistanceMax_1 && instanceThis->config[x].state_1 != true)
                {   
                    
              
                    //ligar somente se o timer de debounce estiver nulo, significa que o ciclo foi completo
                    if(instanceThis->config[x].timer_1 == NULL)
                    {
                        
                         //create task of onled
                        instanceThis->config[x].state_1 = true;
                        FadeParams* fadeparams = new FadeParams;
                        fadeparams->strip = instanceThis->ledStrip;
                        fadeparams->startLed = instanceThis->config[x].startLed_1;
                        fadeparams->endLed = instanceThis->config[x].endLed_1;
                        fadeparams->state = instanceThis->config[x].state_1;
                        fadeparams->semaphoreStrip = instanceThis->stripSemaphore;
                        fadeparams->timer = &instanceThis->config[x].timer_1;
                        fadeparams->color = instanceThis->color;
                   
                        
                        // xTaskCreate(fadeTask, "fadeTask", 1024, fadeparams, 2, NULL);
                        xTaskCreatePinnedToCore(fadeTask, "fadeTask", 2048, fadeparams, 2, NULL, 0);

                        // printf("ligou %d \n", x);
                    }   

                    //se nao reiniciar para não apagar o led 
                    else
                    {
                        xTimerStart(instanceThis->config[x].timer_1, 0);
                    }
                   
                }

                if((instanceThis->config[x].distance < instanceThis->config[x].actionDistanceMin_1||instanceThis->config[x].distance > instanceThis->config[x].actionDistanceMax_1) && instanceThis->config[x].state_1 == true)
                {
                    //create task of offled
                    
                    // xTaskCreate(fadeTask, "fadeTask", 1024, fadeparams, 1, NULL);
                    
                    if(instanceThis->config[x].timer_1 == NULL)
                    {
                        instanceThis->config[x].state_1 = false;
                        
                        FadeParams* fadeparams = new FadeParams;
                        fadeparams->strip = instanceThis->ledStrip;
                        fadeparams->startLed = instanceThis->config[x].startLed_1;
                        fadeparams->endLed = instanceThis->config[x].endLed_1;
                        fadeparams->state = instanceThis->config[x].state_1;
                        fadeparams->semaphoreStrip = instanceThis->stripSemaphore;
                        fadeparams->timer = &instanceThis->config[x].timer_1;
                        fadeparams->color = instanceThis->color;

                        instanceThis->config[x].timer_1 = xTimerCreate(
                            "offStripTask",
                            pdMS_TO_TICKS(2000),
                            pdFALSE,
                            (void *)fadeparams,
                            myTimerCallback
                        );

                        xTimerStart(instanceThis->config[x].timer_1, 0);
                        // printf("deligou %d \n", x);

                    }
                    
            
                }   


                //distance action
                if(instanceThis->config[x].distance > instanceThis->config[x].actionDistanceMin_2 && instanceThis->config[x].distance < instanceThis->config[x].actionDistanceMax_2 && instanceThis->config[x].state_2 != true)
                {   
                    
              
                    //ligar somente se o timer de debounce estiver nulo, significa que o ciclo foi completo
                    if(instanceThis->config[x].timer_2 == NULL)
                    {
                        
                         //create task of onled
                        instanceThis->config[x].state_2 = true;
                        FadeParams* fadeparams = new FadeParams;
                        fadeparams->strip = instanceThis->ledStrip;
                        fadeparams->startLed = instanceThis->config[x].startLed_2;
                        fadeparams->endLed = instanceThis->config[x].endLed_2;
                        fadeparams->state = instanceThis->config[x].state_2;
                        fadeparams->semaphoreStrip = instanceThis->stripSemaphore;
                        fadeparams->timer = &instanceThis->config[x].timer_2;
                        fadeparams->color = instanceThis->color;
                   
                        
                       //xTaskCreate(fadeTask, "fadeTask", 1024, fadeparams, configMAX_PRIORITIES - 1, NULL);
                        xTaskCreatePinnedToCore(fadeTask, "fadeTask", 2048, fadeparams, 2, NULL, 0);
                        //printf("ligou %d \n", x);
                    }   

                    //se nao reiniciar para não apagar o led 
                    else
                    {
                        xTimerStart(instanceThis->config[x].timer_2, 0);
                    }
                   
                }

                if((instanceThis->config[x].distance < instanceThis->config[x].actionDistanceMin_2||instanceThis->config[x].distance > instanceThis->config[x].actionDistanceMax_2) && instanceThis->config[x].state_2 == true)
                {
                    //create task of offled
                    
                    // xTaskCreate(fadeTask, "fadeTask", 1024, fadeparams, 1, NULL);
                    
                    if(instanceThis->config[x].timer_2 == NULL)
                    {
                        instanceThis->config[x].state_2 = false;
                        
                        FadeParams* fadeparams = new FadeParams;
                        fadeparams->strip = instanceThis->ledStrip;
                        fadeparams->startLed = instanceThis->config[x].startLed_2;
                        fadeparams->endLed = instanceThis->config[x].endLed_2;
                        fadeparams->state = instanceThis->config[x].state_2;
                        fadeparams->semaphoreStrip = instanceThis->stripSemaphore;
                        fadeparams->timer = &instanceThis->config[x].timer_2;
                        fadeparams->color = instanceThis->color;

                        instanceThis->config[x].timer_2 = xTimerCreate(
                            "offStripTask",
                            pdMS_TO_TICKS(2000),
                            pdFALSE,
                            (void *)fadeparams,
                            myTimerCallback
                        );

                        xTimerStart(instanceThis->config[x].timer_2, 0);
                        //printf("deligou %d \n", x);
                    }
                    
            
                }
            
                
            }
            xSemaphoreGive(instanceThis->sensorSemaphore);
        }  
       
         vTaskDelay(pdMS_TO_TICKS(150));
     
    }
    
}


    