#include "./service_nvs_save_data_sensor.h"

esp_err_t service_nvs_save_data_sensor::saveSensorData(sensor_distance_data_t *dataSensor, size_t numSensors, struct_color_t *color)
{
    nvs_handle_t nvsHandle;

    //open nvs
    esp_err_t ret = nvs_open("storage",NVS_READWRITE, &nvsHandle);

    if(ret != ESP_OK)
    {
        printf("error open nvs \n");
        return ret;
    }

    //length array
    size_t size = numSensors * sizeof(sensor_distance_data_t);

    //save blob
    ret = nvs_set_blob(nvsHandle,"sensor_data", dataSensor, size);

    if(ret != ESP_OK)
    {
        printf("erro saving sensor data \n");
        nvs_close(nvsHandle);
        return ret;
    } 

    ret = nvs_set_blob(nvsHandle,"color", color, sizeof(struct_color_t));

    if(ret != ESP_OK)
    {
        printf("erro saving color \n");
        nvs_close(nvsHandle);
        return ret;
    } 

    //commit data
    ret = nvs_commit(nvsHandle);
    if(ret != ESP_OK)
    {

        printf("erro commit data \n");
        nvs_close(nvsHandle);
        return ret;

    }

    printf("Sensor data array saved successfully\n");
    nvs_close(nvsHandle);
    return ESP_OK;  // Sucesso
}

esp_err_t service_nvs_save_data_sensor::loadSensorData(sensor_distance_data_t *data, ServiceSensorDistance *serviceDistance, struct_color_t *color)
{
    size_t size = 0;
    nvs_handle_t nvsHandle;

    // Abrir NVS
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvsHandle);
    if(ret != ESP_OK)
    {
        printf("Erro ao abrir NVS: %s\n", esp_err_to_name(ret));
        return ret;
    }

    // Primeiro, obter o tamanho do blob
    ret = nvs_get_blob(nvsHandle, "sensor_data", NULL, &size);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        printf("Dados não encontrados na NVS.\n");
        nvs_close(nvsHandle);
        return ret;
    } else if (ret != ESP_OK) {
        printf("Erro ao obter o tamanho do blob: %s\n", esp_err_to_name(ret));
        nvs_close(nvsHandle);
        return ret;
    }

    // Certificar-se de que o tamanho é compatível com o buffer
    if (size > 0) {
        // Agora, recuperar os dados
        ret = nvs_get_blob(nvsHandle, "sensor_data", data, &size);
        if (ret != ESP_OK) {
            printf("Erro ao obter dados do sensor: %s\n", esp_err_to_name(ret));
            nvs_close(nvsHandle);
            return ret;
        }
        printf("Sensor data array get successfully\n");
    } 

    // Primeiro, obter o tamanho do blob
    ret = nvs_get_blob(nvsHandle, "color", NULL, &size);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        printf("Dados não encontrados na NVS.\n");
        nvs_close(nvsHandle);
        return ret;
    } else if (ret != ESP_OK) {
        printf("Erro ao obter o tamanho do blob: %s\n", esp_err_to_name(ret));
        nvs_close(nvsHandle);
        return ret;
    }

    // Certificar-se de que o tamanho é compatível com o buffer
    if (size > 0) {
        // Agora, recuperar os dados
        ret = nvs_get_blob(nvsHandle, "color", color, &size);
        if (ret != ESP_OK) {
            printf("Erro ao obter dados da color: %s\n", esp_err_to_name(ret));
            nvs_close(nvsHandle);
            return ret;
        }
        printf("Sensor data color get successfully\n");
    } 
    
    nvs_close(nvsHandle);

    // Reiniciar tasks
    vTaskDelete(serviceDistance->sensorTaskHandler);
    serviceDistance->sensorTaskHandler = NULL;
    vTaskDelete(serviceDistance->controlTaskHandler);
    serviceDistance->controlTaskHandler = NULL;
    serviceDistance->startTasks();
    
    return ESP_OK;  // Sucesso
}