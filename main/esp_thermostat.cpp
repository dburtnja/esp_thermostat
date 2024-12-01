#include <iostream>
#include <hal/gpio_types.h>
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>

#include "NvsStorage.h"
#include "Led.h"
#include "Thermometer.h"

#define HEATER_GPIO GPIO_NUM_5

enum HeaterState {
    OFF = 0,
    ON
};

void init_heater() {
    gpio_reset_pin(HEATER_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(HEATER_GPIO, GPIO_MODE_OUTPUT);
}

void set_heater(HeaterState state) {
    gpio_set_level(HEATER_GPIO, state);
}

extern "C" void app_main(void)
{
    NvsStorage nvs_storage{};
    Led led{};
    Thermometer thermometer{};

    led.init();

    if (!nvs_storage.init()) {
        led.fast_blinking();
        printf("Error: can't initialise NVS.\n");
        return;
    }

    if (!thermometer.init()) {
        led.fast_blinking();
        printf("Error: can't initialise thermometer.\n");
        return;
    }

    auto temperature_threshold = nvs_storage.get_temperature_threshold();
    if (!temperature_threshold) {
        printf("Error: can't read temperature threshold.\n");
        led.fast_blinking();
        return;
    }

    while (true) {
        led.on();
        auto temperature = thermometer.get_value();

        if (!temperature) {
            led.fast_blinking();
            printf("Error: can't read temperature.\n");
        }

        if (*temperature < *temperature_threshold) {
            set_heater(ON);
        } else {
            set_heater(OFF);
        }

        vTaskDelay(1000);
        led.off();
        vTaskDelay(10000);
    }
}
