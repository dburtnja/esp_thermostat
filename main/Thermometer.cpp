#include <esp_log.h>
#include "Thermometer.h"
#include "onewire_bus.h"

#define THERMOMETER_GPIO 15

bool Thermometer::init() noexcept {
    // install new 1-wire bus
    onewire_bus_handle_t bus;
    onewire_bus_config_t bus_config = {
            .bus_gpio_num = THERMOMETER_GPIO,
    };
    onewire_bus_rmt_config_t rmt_config = {
            .max_rx_bytes = 10, // 1byte ROM command + 8byte ROM number + 1byte device command
    };
    ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));
    ESP_LOGI(__FILENAME__, "1-Wire bus installed on GPIO%d", THERMOMETER_GPIO);

    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_onewire_device;
    esp_err_t search_result = ESP_OK;

    // create 1-wire device iterator, which is used for device search
    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
    ESP_LOGI(__FILENAME__, "Device iterator created, start searching...");

    search_result = onewire_device_iter_get_next(iter, &next_onewire_device);
    if (search_result == ESP_OK) { // found a new device, let's check if we can upgrade it to a DS18B20
        ds18b20_config_t ds_cfg = {};
        if (ds18b20_new_device(&next_onewire_device, &ds_cfg, &device_) == ESP_OK) {
            ESP_LOGI(__FILENAME__, "Found a DS18B20, address: %016llX", next_onewire_device.address);
        } else {
            ESP_LOGI(__FILENAME__, "Found an unknown device, address: %016llX", next_onewire_device.address);
            return false;
        }
    } else {
        return false;
    }

    ESP_ERROR_CHECK(onewire_del_device_iter(iter));
    ESP_ERROR_CHECK(ds18b20_set_resolution(device_, DS18B20_RESOLUTION_12B));

    return true;
}

#define UNEXPECTED_TEMPERATURE 5000

float Thermometer::get_value() const noexcept {
    float temperature{UNEXPECTED_TEMPERATURE};

    ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(device_));
    ESP_ERROR_CHECK(ds18b20_get_temperature(device_, &temperature));

    return temperature;
}
