#ifndef ESP_THERMOSTAT_NVSSTORAGE_H
#define ESP_THERMOSTAT_NVSSTORAGE_H

#include <nvs.h>
#include <optional>

class NvsStorage{
public:
    bool init() noexcept;

    [[nodiscard]] float get_temperature_threshold() const noexcept;
    uint8_t get_start_time_threshold() const noexcept;
    uint8_t get_end_time_threshold() const noexcept;

    bool set_temperature_threshold(float threshold) noexcept;
    bool set_start_time_threshold(int8_t time) noexcept;
    bool set_end_time_threshold(int8_t time) noexcept;

    bool smart_write(const std::string& key, const std::string& value) noexcept;

private:
    const char* STORAGE_NAME{"thermostat_data"};
    const char* TEMPERATURE_KEY{"temperature"};
    const char* START_TIME_KEY{"start_time"};
    const char* END_TIME_KEY{"end_time"};

    nvs_handle_t nvs_handle_;

    mutable std::optional<float> cashed_temperature_threshold_{};
    mutable std::optional<int8_t> cashed_start_time_threshold_{};
    mutable std::optional<int8_t> cashed_end_time_threshold_{};

private:
    uint8_t read_time(const char *key, std::optional<int8_t> &cashed_value) const;
    bool write_time(const char *key, int8_t value) noexcept;
};


#endif //ESP_THERMOSTAT_NVSSTORAGE_H
