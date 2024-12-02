#include <iostream>
#include <hal/gpio_types.h>
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <esp_log.h>

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
        ESP_LOGI(__FILENAME__, "Error: can't initialise NVS.\n");
        led.fast_blinking_blocking_call();
    }

    if (!thermometer.init()) {
        ESP_LOGI(__FILENAME__, "Error: can't initialise thermometer.\n");
        led.fast_blinking_blocking_call();
        return;
    }

    auto temperature_threshold = nvs_storage.get_temperature_threshold();
    if (!temperature_threshold) {
        ESP_LOGI(__FILENAME__, "Error: can't read temperature threshold.\n");
        led.fast_blinking_blocking_call();
        return;
    }

    led.slow_blinking();

    while (true) {
        auto temperature = thermometer.get_value();

        std::cout << "Current temperature: " << temperature << std::endl;
        std::cout << "Temperature threshold: " << *temperature_threshold << std::endl;

        if (temperature < *temperature_threshold) {
            led.on();
            set_heater(ON);
        } else {
            led.off();
            set_heater(OFF);
        }

        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
