#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <driver/gpio.h>

class LedController {
    public:
        LedController(gpio_num_t pin); // Construtor que recebe o pino do LED
        void turnOn();   // Método para acender o LED
        void turnOff();  // Método para apagar o LED

    private:
        gpio_num_t pin; // Pino do LED
};

#endif // LEDCONTROLLER_H
