#include "service_http_server_sensor_distance.h"
#include "services/service_nvs_save_data_sensor/service_nvs_save_data_sensor.h"

struct reqData{
    sensor_distance_data_t * sensor;
    size_t numSensor;
    struct_color_t *color;
    ServiceSensorDistance *serviceDistance;
};

httpd_handle_t server = NULL;
reqData* data = new reqData;

/* responsavel por iniciar os endpoints*/
httpd_handle_t service_http_server_sensor_distance::start_webserver (sensor_distance_data_t * sensors, size_t numSensors, struct_color_t *color, ServiceSensorDistance *serviceDistance)
{   
    server = NULL;
    data = new reqData;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    data->sensor = sensors;
    data->numSensor = numSensors;
    data->color = color;
    data->serviceDistance = serviceDistance;

    //aumentar pilha
    config.stack_size = 8192;

    // servidor sse distance
    if (httpd_start(&server, &config) == ESP_OK) {

        httpd_uri_t getDistance = {
            .uri      = "/getDistance",           // Endpoint do SSE
            .method   = HTTP_GET,         // Método GET para SSE
            .handler  = service_http_server_sensor_distance::getDistance,      // Handler da função SSE
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &getDistance);

        httpd_uri_t getPage = {
            .uri      = "/getPage",           // Endpoint do SSE
            .method   = HTTP_GET,         // Método GET para SSE
            .handler  = service_http_server_sensor_distance::getHtmlPage,      // Handler da função SSE
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &getPage);

        httpd_uri_t getSaveConfig = {
            .uri      = "/getSaveConfig",           // Endpoint do SSE
            .method   = HTTP_GET,         // Método GET para SSE
            .handler  = service_http_server_sensor_distance::saveConfig,      // Handler da função SSE
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &getSaveConfig);

        httpd_uri_t getLoadConfig = {
            .uri      = "/getLoadConfig",           // Endpoint do SSE
            .method   = HTTP_GET,         // Método GET para SSE
            .handler  = service_http_server_sensor_distance::loadConfig,      // Handler da função SSE
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &getLoadConfig);

        //get sensors
        httpd_uri_t get_sensor = {
            .uri = "/sensor",
            .method = HTTP_GET,
            .handler = service_http_server_sensor_distance::getSensors,
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &get_sensor);

        // // CORS
        // httpd_uri_t options_uri_sensor = {
        //     .uri = "/update_sensor", // Especificar a rota do PUT
        //     .method = HTTP_OPTIONS,
        //     .handler = service_http_server_sensor_distance::optionsHandler,
        //     .user_ctx = data
        // };
        // httpd_register_uri_handler(server, &options_uri_sensor);

        //put sensor
        httpd_uri_t put_sensor_distance = {
            .uri = "/update_sensor",
            .method = HTTP_PUT,
            .handler = service_http_server_sensor_distance::putDistanceSensor,
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &put_sensor_distance);


        // // CORS
        // httpd_uri_t options_uri_strip = {
        //     .uri = "/update_rstrip", // Especificar a rota do PUT
        //     .method = HTTP_OPTIONS,
        //     .handler = service_http_server_sensor_distance::optionsHandler,
        //     .user_ctx = data
        // };

        // httpd_register_uri_handler(server, &options_uri_strip);

        //put strip
        httpd_uri_t put_range_strip = {
            .uri = "/update_rstrip",
            .method = HTTP_PUT,
            .handler = service_http_server_sensor_distance::putRangeStrip,
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &put_range_strip);

        // // CORS
        // httpd_uri_t options_uri_color = {
        //     .uri = "/updateCStrip", // Especificar a rota do PUT
        //     .method = HTTP_OPTIONS,
        //     .handler = service_http_server_sensor_distance::optionsHandler,
        //     .user_ctx = data
        // };
        // httpd_register_uri_handler(server, &options_uri_color);
        
        //switchcolor
        httpd_uri_t put_color_strip = {
            .uri = "/updateCStrip",
            .method = HTTP_PUT,
            .handler = service_http_server_sensor_distance::putColorStrip,
            .user_ctx = data
        };
        httpd_register_uri_handler(server, &put_color_strip);
    }
    return server;
}

esp_err_t service_http_server_sensor_distance::getDistance(httpd_req_t *req)
{   
    // Recupera o contexto da requisição
    reqData *context = (reqData*) req->user_ctx;
    cJSON *json = cJSON_CreateObject();
    cJSON *sensorarray =  cJSON_CreateArray();

    for(int i = 0; i < context->numSensor; i++)
    {   

        cJSON *tempObject = cJSON_CreateObject();
        cJSON_AddNumberToObject(tempObject, "sensor", i);
        cJSON_AddNumberToObject(tempObject, "distance", context->sensor[i].distance);

        //add to array
        cJSON_AddItemToArray(sensorarray,tempObject);
    }

    cJSON_AddItemToObject(json, "sensors", sensorarray);

    char* jsonString = cJSON_Print(json);

    //response

    //headers 
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");

    //cors
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

    esp_err_t ret = httpd_resp_send(req, jsonString, strlen(jsonString));


    //free memory
    cJSON_Delete(json);
    free(jsonString);

    return ret;
}

esp_err_t service_http_server_sensor_distance :: getSensors(httpd_req_t *req)
{
     // Recupera o contexto da requisição, pegar o objeto passado na definicao do endpoint
    reqData *context = (reqData*) req->user_ctx;

    //create json object
    cJSON *json = cJSON_CreateObject();

    cJSON *sensorarray =  cJSON_CreateArray();

    for(int i = 0; i < context->numSensor; i++)
    {   

        cJSON *tempObject = cJSON_CreateObject();
        cJSON_AddNumberToObject(tempObject, "sensor", i);
        cJSON_AddNumberToObject(tempObject, "actionDistanceMin1", context->sensor[i].actionDistanceMin_1);
        cJSON_AddNumberToObject(tempObject, "actionDistanceMax1", context->sensor[i].actionDistanceMax_1);
        cJSON_AddNumberToObject(tempObject, "actionDistanceMin2", context->sensor[i].actionDistanceMin_2);
        cJSON_AddNumberToObject(tempObject, "actionDistanceMax2", context->sensor[i].actionDistanceMax_2);
        cJSON_AddNumberToObject(tempObject, "startLed1", context->sensor[i].startLed_1);
        cJSON_AddNumberToObject(tempObject, "endLed1", context->sensor[i].endLed_1);
        cJSON_AddNumberToObject(tempObject, "startLed2", context->sensor[i].startLed_2);
        cJSON_AddNumberToObject(tempObject, "endLed2", context->sensor[i].endLed_2);
        cJSON_AddNumberToObject(tempObject, "gpioTrig", context->sensor[i].sensor.trigger_pin);
        cJSON_AddNumberToObject(tempObject, "gpioEcho", context->sensor[i].sensor.echo_pin);
        
        //add to array
        cJSON_AddItemToArray(sensorarray,tempObject);
    }
    // add in main object
    cJSON_AddItemToObject(json, "sensors", sensorarray);

    char* jsonString = cJSON_Print(json);

    //headers 
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");

    //cors
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

    //response
    esp_err_t ret = httpd_resp_send(req, jsonString, strlen(jsonString));

    //free memory
    cJSON_Delete(json);
    free(jsonString);

    return ret;
}


esp_err_t service_http_server_sensor_distance::putDistanceSensor(httpd_req_t *req)
{
    reqData *context = (reqData*) req->user_ctx;

    // CORS
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type, Authorization");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");

    char query[256];
    char param[32];

    int sensor;
    int maxDistance;
    int minDistance;
    int stage;
    
    // Extrai a query string da URL
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        // Extrai os parâmetros
        httpd_query_key_value(query, "sensor", param, sizeof(param));
        sensor = atoi(param);

        httpd_query_key_value(query, "stage", param, sizeof(param));
        stage = atoi(param);

        httpd_query_key_value(query, "min_distance", param, sizeof(param));
        minDistance = atoi(param);

        httpd_query_key_value(query, "max_distance", param, sizeof(param));
        maxDistance = atoi(param);

        service_calibrate_distance_sensor::calibrateDistanceSensor(&context->sensor[sensor], minDistance, maxDistance, stage);
        return httpd_resp_send(req, HTTPD_200, NULL);
    } else {
        // Retorna erro se a query string não estiver presente
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, NULL);
        return ESP_FAIL;
    }
}



esp_err_t service_http_server_sensor_distance::putRangeStrip(httpd_req_t *req)
{
    reqData *context = (reqData*) req->user_ctx;

    // CORS
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type, Authorization");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");

    char query[256];
    char param[32];

    int sensor;
    int startLed;
    int endLed;
    int stage;
    
    // Extrai a query string da URL
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {

        // Extrai os parâmetros
        httpd_query_key_value(query, "sensor", param, sizeof(param));
        sensor = atoi(param);

        httpd_query_key_value(query, "stage", param, sizeof(param));
        stage = atoi(param);

        httpd_query_key_value(query, "start_led", param, sizeof(param));
        startLed = atoi(param);

        httpd_query_key_value(query, "end_led", param, sizeof(param));
        endLed = atoi(param);

        // Executa a função de calibração com os parâmetros extraídos
        service_calibrate_distance_sensor::calibrateAroundLed(&context->sensor[sensor], startLed, endLed, stage);

        // Envia resposta 200 OK
        return httpd_resp_send(req, HTTPD_200, NULL);
       
    } else {
        // Retorna erro 400 se a query string não estiver presente
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad request");
        return ESP_FAIL;
    }
}



esp_err_t service_http_server_sensor_distance :: putColorStrip(httpd_req_t *req)
{
    reqData *context = (reqData*) req->user_ctx;

    // CORS
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type, Authorization");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");


    char query[256];
    char param[32];

    int red;
    int blue;
    int green;
 
    
    // Extrai a query string da URL
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {

        // Extrai o parâmetro 
       httpd_query_key_value(query, "red", param, sizeof(param));
       red = atoi(param);

       httpd_query_key_value(query, "blue", param, sizeof(param));
       blue = atoi(param);

       httpd_query_key_value(query, "green", param, sizeof(param));
       green = atoi(param);

       service_calibrate_distance_sensor::changeColor(context->color,red,blue,green);
       return httpd_resp_send(req, HTTPD_200,NULL);
       
    } 
    
    else 
    {
        // Retorna erro se a query string não estiver presente
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,NULL);
        return ESP_FAIL;
    }


}


esp_err_t service_http_server_sensor_distance::optionsHandler(httpd_req_t *req) {
    // Configura os cabeçalhos para CORS
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type, Authorization");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    return httpd_resp_send(req, NULL, 0); // Resposta vazia
}

esp_err_t service_http_server_sensor_distance::getHtmlPage(httpd_req_t *req) {

    //headers 
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");

    //cors
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

    httpd_resp_set_type(req, "text/html");

    // Envia a resposta
    httpd_resp_sendstr(req, index_html);

    return ESP_OK;
}

esp_err_t service_http_server_sensor_distance::saveConfig(httpd_req_t *req) {

    //Recupera o contexto da requisição
    reqData *context = (reqData*) req->user_ctx;

    //headers 
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");

    //cors
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

    esp_err_t ret = service_nvs_save_data_sensor::saveSensorData(context->sensor,context->numSensor,context->color);

    if(ret == ESP_OK)
    {
         httpd_resp_sendstr(req, "OK");  // Envia resposta "OK"
    }
    else
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
    }

    return ESP_OK;
}

esp_err_t service_http_server_sensor_distance::loadConfig(httpd_req_t *req) {

    //Recupera o contexto da requisição
    reqData *context = (reqData*) req->user_ctx;

    //headers 
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");

    //cors
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

    
    esp_err_t ret = service_nvs_save_data_sensor::loadSensorData(context->sensor,context->serviceDistance,context->color);

    if(ret == ESP_OK)
    {
         httpd_resp_sendstr(req, "OK");  // Envia resposta "OK"
    }
    else
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
    }

    return ESP_OK;
}


// void service_http_server_sensor_distance::createTaskWebServer(void *pvParameters) {
//     struct_task_t *instanceThis = static_cast<struct_task_t *>(pvParameters);

//     // Inicia o servidor HTTP
//     service_http_server_sensor_distance::start_webserver(instanceThis->sensors, instanceThis->numSensors, instanceThis->color);

//     // Loop infinito para manter a tarefa ativa
//     while (true) {
//         // Aqui você pode adicionar lógica, como verificações ou atualizações periódicas.
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo
//     }
// }

void service_http_server_sensor_distance::stopWebserver() {
    esp_err_t err = httpd_stop(server);
    server = NULL;
    delete(data);
}


