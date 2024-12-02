#ifndef ESP_THERMOSTAT_NVSSTORAGE_H
#define ESP_THERMOSTAT_NVSSTORAGE_H

#include <nvs.h>
#include <optional>

class NvsStorage{
public:
    bool init() noexcept;

    [[nodiscard]] std::optional<float> get_temperature_threshold() const noexcept;

    bool set_temperature_threshold(float threshold) noexcept;

private:
    const char* STORAGE_NAME{"thermostat_data"};
    const char* TEMPERATURE_KEY{"temperature"};
    const char* START_TIME_KEY{"start_time"};
    const char* end_TIME_KEY{"end_time"};

    nvs_handle_t nvs_handle_;

    mutable std::optional<float> cashed_temperature_threshold{};
};


#endif //ESP_THERMOSTAT_NVSSTORAGE_H
