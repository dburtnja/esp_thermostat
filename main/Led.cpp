#include "Led.h"

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define BLINK_GPIO GPIO_NUM_2

void Led::init() noexcept {
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

void Led::fast_blinking() noexcept {
    task = std::thread([](){
        while (true) {
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(1000);
            gpio_set_level(BLINK_GPIO, 2);
        }
    });
}

void Led::on() noexcept {
    gpio_set_level(BLINK_GPIO, 1);
}

void Led::off() noexcept {
    gpio_set_level(BLINK_GPIO, 0);

}
