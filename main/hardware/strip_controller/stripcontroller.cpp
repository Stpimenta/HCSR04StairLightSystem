#include "stripcontroller.h"
static const char *TAG = "HardwareLedStrip";


stripController::
stripController(gpio_num_t dataPin, uint32_t ledNumbers, uint32_t cyclesHz)
    :dataPin(dataPin),ledNumbers(ledNumbers), cyclesHz(cyclesHz)
{   
    //config object
    led_strip_config_t strip_config = {

        .strip_gpio_num = dataPin,
        .max_leds = ledNumbers,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
    };

    //config rmt for communication
    led_strip_rmt_config_t rmt_config = {
        #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
                .rmt_channel = 0,
        #else
                .clk_src = RMT_CLK_SRC_DEFAULT,
                .resolution_hz = cyclesHz * 1000 * 1000,
        #endif
    };

    //init led strip
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &ledStrip));
    ESP_LOGI(TAG, "init ok");
  
}

void delayWithoutYield(uint32_t delay_ms) {
    uint32_t start_time = xTaskGetTickCount(); // Obtém o tempo atual
    while ((xTaskGetTickCount() - start_time) < delay_ms / portTICK_PERIOD_MS) {
        // Loop vazio - apenas espera
    }
}


void stripController :: stripFade (int red, int green, int blue, uint32_t fade_delay_ms)
{   
    //shared var for loops
    int i;
    for (i = 0; i <= 255; i++) {

        //calc fade
        int r = (red * i) / 255;
        int g = (green * i) / 255;
        int b = (blue * i) / 255;
        
        //set color
        for (int j = 0; j < ledNumbers; j++) {
            
            ESP_ERROR_CHECK(led_strip_set_pixel(ledStrip, j, r, g, b));
        }
        
        ESP_ERROR_CHECK(led_strip_refresh(ledStrip));
        vTaskDelay(pdMS_TO_TICKS(fade_delay_ms));
    }
    

    for (i = 255; i >= 0; i--) {
         //calc fade
        int r = (red * i) / 255;
        int g = (green * i) / 255;
        int b = (blue * i) / 255;
        
          //set color
        for (int j = 0; j < ledNumbers; j++) {
            ESP_ERROR_CHECK(led_strip_set_pixel(ledStrip, j, r, g, b));
        }
        
        ESP_ERROR_CHECK(led_strip_refresh(ledStrip));
        vTaskDelay(pdMS_TO_TICKS(fade_delay_ms));
    }
}

void stripController::onFade(int red, int blue, int green, uint32_t startLed, uint32_t endLed, uint32_t fade_delay_ms)
{
    // Fade in from startLed to endLed
    for (int i = 0; i <= 75; i++) {
        // Calculate fade color
        int r = (red * i) / 75;
        int g = (green * i) / 75;
        int b = (blue * i) / 75;
        
        // Set color for specified range of LEDs
        for (int j = startLed; j <= endLed; j++) {
            ESP_ERROR_CHECK(led_strip_set_pixel(ledStrip, j, r, g, b));
        }
        
        ESP_ERROR_CHECK(led_strip_refresh(ledStrip));
        esp_rom_delay_us(1);
    }
}

void stripController::offFade(int red, int blue, int green, uint32_t startLed, uint32_t endLed, uint32_t fade_delay_ms)
{
    // Fade out from startLed to endLed
    for (int i = 75; i >= 0; i--) {
        // Calculate fade color
        int r = (red * i) / 75;
        int g = (green * i) / 75;
        int b = (blue * i) / 75;
        
        // Set color for specified range of LEDs
        for (int j = startLed; j <= endLed; j++) {
            ESP_ERROR_CHECK(led_strip_set_pixel(ledStrip, j, r, g, b));
        }
        
        ESP_ERROR_CHECK(led_strip_refresh(ledStrip));
        esp_rom_delay_us(1);
    }
}