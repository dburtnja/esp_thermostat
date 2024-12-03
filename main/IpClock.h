#ifndef ESP_THERMOSTAT_IPCLOCK_H
#define ESP_THERMOSTAT_IPCLOCK_H

class IpClock {
public:
    bool init() noexcept;
    [[nodiscard]] int8_t get_current_hour() const noexcept;
    [[nodiscard]] std::string to_string() const noexcept;
};


#endif //ESP_THERMOSTAT_IPCLOCK_H
