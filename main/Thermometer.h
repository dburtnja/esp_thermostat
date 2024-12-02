#ifndef ESP_THERMOSTAT_THERMOMETER_H
#define ESP_THERMOSTAT_THERMOMETER_H

#include <optional>
#include "ds18b20.h"


class Thermometer{
public:
    bool init() noexcept;

    [[nodiscard]] float get_value() const noexcept;

private:
    ds18b20_device_handle_t device_{};
};


#endif //ESP_THERMOSTAT_THERMOMETER_H
