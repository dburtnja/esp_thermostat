#include "Heater.h"

#include <hal/gpio_types.h>
#include <driver/gpio.h>
#include <iostream>

#define HEATER_GPIO GPIO_NUM_13

void Heater::init() noexcept {
    gpio_reset_pin(HEATER_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(HEATER_GPIO, GPIO_MODE_OUTPUT);
}

void Heater::set(HeaterState state) noexcept {
    std::cout << "Set heater " << state << std::endl;
    gpio_set_level(HEATER_GPIO, state);
    state_ = state;
}

std::string Heater::to_string() const noexcept {
    if (state_ == ON) {
        return "ON";
    } else {
        return "OFF";
    }
}
