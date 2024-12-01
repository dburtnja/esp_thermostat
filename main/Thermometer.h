#ifndef ESP_THERMOSTAT_THERMOMETER_H
#define ESP_THERMOSTAT_THERMOMETER_H

#include <optional>

class Thermometer{
public:
    bool init() noexcept;

    [[nodiscard]] std::optional<float> get_value() const noexcept;
};


#endif //ESP_THERMOSTAT_THERMOMETER_H
