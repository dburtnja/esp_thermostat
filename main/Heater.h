#ifndef ESP_THERMOSTAT_HEATER_H
#define ESP_THERMOSTAT_HEATER_H

#include <string>

enum HeaterState {
    ON = 0,
    OFF = 1
};

class Heater {
public:
    void init() noexcept;
    void set(HeaterState state) noexcept;
    [[nodiscard]] std::string to_string() const noexcept;

private:
    HeaterState state_{};
};


#endif //ESP_THERMOSTAT_HEATER_H
