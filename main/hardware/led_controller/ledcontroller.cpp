// ledcontroller.cpp
#include "ledcontroller.h"

LedController::LedController(gpio_num_t pin) : pin(pin) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void LedController::turnOn() {
    gpio_set_level(pin, 1);
}

void LedController::turnOff() {
    gpio_set_level(pin, 0);
}
