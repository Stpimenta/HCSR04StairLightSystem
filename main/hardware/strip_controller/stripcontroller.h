#ifndef STRIP_CONTROLLER_H
#define STRIP_CONTROLLER_H

#include <driver/gpio.h>
#include <led_strip.h>
#include "esp_log.h"

class stripController
{

    private:
       gpio_num_t dataPin;
       uint32_t ledNumbers;
       uint32_t cyclesHz;
       led_strip_handle_t ledStrip;

    public:
        stripController(gpio_num_t dataPin, uint32_t ledNumbers, uint32_t cyclesHz);
        void stripFade(int red, int green, int blue, uint32_t fade_delay_ms);
        void onFade(int red, int green, int blue, uint32_t startLed, uint32_t end_Led, uint32_t fade_delay_ms);
        void offFade(int red, int green, int blue, uint32_t startLed, uint32_t end_Led, uint32_t fade_delay_ms);
};

#endif 