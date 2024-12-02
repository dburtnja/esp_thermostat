#include "Led.h"

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iostream>

#define BLINK_GPIO GPIO_NUM_2

void Led::init() noexcept {
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

void Led::fast_blinking_blocking_call() noexcept {
    while (true) {
        blink_interval_ms(150);
    }
}

void Led::on() noexcept {
    std::cout << "Led ON!" << std::endl;
    is_on = true;
    gpio_set_level(BLINK_GPIO, 1);
}

void Led::off() noexcept {
    std::cout << "Led OFF!" << std::endl;
    is_on = false;
    gpio_set_level(BLINK_GPIO, 0);
}

void Led::slow_blinking() {
    task_ = std::thread([&]{
        while (true) {
            if (is_on) {
                vTaskDelay(pdMS_TO_TICKS(2000));
                continue;
            }

            blink_interval_ms(1000);
        }
    });
}

void Led::blink_interval_ms(uint32_t interval) {
    gpio_set_level(BLINK_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(interval));
    gpio_set_level(BLINK_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(interval));
}
