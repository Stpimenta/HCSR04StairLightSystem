#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <ultrasonic.h>
#include <hardware/led_controller/ledcontroller.h> 
#include <services/service_sensor_distance_led/service_sensor_distance.h>
#include <services/service_sensor_distance_led/struct/struct_sensor_distance_data.h>
#include "hardware/strip_controller/stripcontroller.h"
#include "services/service_calibrate_distance_sensor/service_calibrate_distance_sensor.h"
#include "services/service_sensor_distance_led/struct/struct_color.h"
#include "services/service_connect_wifi/service_connect_wifi.h"
#include "esp_http_server.h"
#include "services/service_http_server/service_http_server_sensor_distance.h"
#include "services/service_nvs_save_data_sensor/service_nvs_save_data_sensor.h"
#include "esp_wifi.h"
#include "env.h"  


 /* Instância de configuração do sensor */
size_t numSensors = 1;
sensor_distance_data_t* sensorData = new sensor_distance_data_t[numSensors];


 /* Configuração de cores */
struct_color_t* color = new struct_color_t;




// btn interrupt wifi
#define BTNGPIO GPIO_NUM_14
#define DEBOUNCE_TIME_MS 10000 

static volatile bool state = false;

//Variável para armazenar o tempo da última interrupção
static volatile uint32_t lastInterruptTime = 0;

//Acionar outra task
static TaskHandle_t TaskWifiManagerkHandle = NULL; // Handle da tarefa

static void IRAM_ATTR button_isr_handler(void* arg) {
    uint32_t currentTime = xTaskGetTickCountFromISR(); // Tempo atual em ticks
    if (currentTime - lastInterruptTime > pdMS_TO_TICKS(DEBOUNCE_TIME_MS)) {
        // Se o tempo passado desde a última interrupção for maior que o tempo de debounce, processa o evento
        lastInterruptTime = currentTime; // Atualiza o tempo da última interrupção
        xTaskNotifyGive(TaskWifiManagerkHandle); // Notifica a tarefa que o botão foi pressionado
    }
}

//task para nao sobrecarregar a interrupção
void TaskWifiManager(void *pvParameter) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Aguarda notificação
        if (state) {
            service_connect_wifi::wifi_reconnect();
        } else {

            service_http_server_sensor_distance::stopWebserver(); // sensorData[1].sensor.trigger_pin = GPIO_NUM_16;
            service_connect_wifi::wifi_desconnect();
           
        }
        state = !state; // Alterna o estado
    }
}

extern "C" void app_main(void) {   

    // Inicialização dos sensores
    sensorData[0].sensor.trigger_pin = GPIO_NUM_18;
    sensorData[0].sensor.echo_pin = GPIO_NUM_19;
    sensorData[0].distance = 0;
    sensorData[0].state_1 = false;
    sensorData[0].actionDistanceMin_1 = 1;
    sensorData[0].actionDistanceMax_1 = 20;
    sensorData[0].timer_1 = NULL;
    sensorData[0].startLed_1 = 1;
    sensorData[0].endLed_1 = 9;

    sensorData[0].state_2 = false;
    sensorData[0].actionDistanceMin_2 = 21;
    sensorData[0].actionDistanceMax_2 = 40;
    sensorData[0].timer_2 = NULL;
    sensorData[0].startLed_2 = 10;
    sensorData[0].endLed_2 = 19;

    // sensorData[1].sensor.trigger_pin = GPIO_NUM_16;
    // sensorData[1].sensor.echo_pin = GPIO_NUM_4;
    // sensorData[1].distance = 0;
    // sensorData[1].state_1 = false;
    // sensorData[1].actionDistanceMin_1 = 1;
    // sensorData[1].actionDistanceMax_1 = 20;
    // sensorData[1].timer_1 = NULL;
    // sensorData[1].startLed_1 = 20;
    // sensorData[1].endLed_1 = 29;

    // sensorData[1].state_2 = false;
    // sensorData[1].actionDistanceMin_2 = 21;
    // sensorData[1].actionDistanceMax_2 = 40;
    // sensorData[1].timer_2 = NULL;
    // sensorData[1].startLed_2 = 30;
    // sensorData[1].endLed_2 = 39;

    /*color*/
    color->red = 155;
    color->green = 155;
    color->blue = 155;

   
    /* Instanciar a fita */
    stripController* stripLed = new stripController(GPIO_NUM_21, 300, 80);

    // Instanciar o serviço de distância
    ServiceSensorDistance* serviceSensorDistance = new ServiceSensorDistance(sensorData, numSensors, stripLed, color);
    serviceSensorDistance->startTasks();

    // start wifi
    service_connect_wifi::initialize_nvs();
    service_connect_wifi::wifi_connection(ssid, pass, device);

    // start http server
    service_http_server_sensor_distance::start_webserver(sensorData, numSensors, color, serviceSensorDistance);

    // Registrar o botão
    gpio_set_direction(BTNGPIO, GPIO_MODE_INPUT); // Entrada
    gpio_set_pull_mode(BTNGPIO, GPIO_PULLUP_ONLY); // Sempre em 3.3V
    gpio_set_intr_type(BTNGPIO, GPIO_INTR_NEGEDGE); // Flutuação de 1 para 0

    // Instalar a interrupção
    gpio_install_isr_service(0); // Habilita
    gpio_isr_handler_add(BTNGPIO, button_isr_handler, NULL); // Handler

    // Criação da tarefa
    xTaskCreate(TaskWifiManager, "WiFi Task", 4096, NULL, 1, &TaskWifiManagerkHandle);

    //load config
    bool loadConfig = true;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if(loadConfig)
        {
            service_nvs_save_data_sensor::loadSensorData(sensorData,serviceSensorDistance,color);
            loadConfig = false;
        }
    
    }
}
