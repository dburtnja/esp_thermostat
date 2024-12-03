#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include "NvsStorage.h"

#include "nvs_flash.h"

#define DEFAULT_TEMPERATURE_THRESHOLD 21.5f
#define DEFAULT_START_TIME_THRESHOLD 5
#define DEFAULT_END_TIME_THRESHOLD 20

#define UNEXPECTED_TEMPERATURE 5000.0f

bool are_equal(float a, float b, float epsilon = 1e-6f) {
    return std::fabs(a - b) < epsilon;
}

bool NvsStorage::init() noexcept{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    esp_err_t err = nvs_open(STORAGE_NAME, NVS_READWRITE, &nvs_handle_);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));

        return false;
    }

    auto temp_threshold = get_temperature_threshold();

    if (!are_equal(temp_threshold, UNEXPECTED_TEMPERATURE)) {
        // Setting default values
        if (!set_temperature_threshold(DEFAULT_TEMPERATURE_THRESHOLD) ||
            !set_start_time_threshold(DEFAULT_START_TIME_THRESHOLD) ||
            !set_end_time_threshold(DEFAULT_END_TIME_THRESHOLD)) {
            return false;
        }
    }

    return true;
}

bool NvsStorage::set_temperature_threshold(float threshold) noexcept {
    std::cout << "set_temperature NVS " << threshold << std::endl;

    esp_err_t err = nvs_set_blob(nvs_handle_, TEMPERATURE_KEY, &threshold, sizeof(threshold));
    if (err != ESP_OK) {
        printf("Failed to save date!\n");
        return false;
    }

    err = nvs_commit(nvs_handle_);
    if (err != ESP_OK) {
        printf("Failed to commit date!\n");
        return false;
    }

    cashed_temperature_threshold_ = threshold;
    return true;
}

float NvsStorage::get_temperature_threshold() const noexcept {
    if (cashed_temperature_threshold_) {
        return *cashed_temperature_threshold_;
    }

    size_t size{sizeof(float)};
    float result;

    esp_err_t err = nvs_get_blob(nvs_handle_, TEMPERATURE_KEY,
                                 &result, &size);

    printf("Read t=%f, data_size=%ui", result, size);
    if (err == ESP_OK) {
        cashed_temperature_threshold_ = result;
        return result;
    }

    return UNEXPECTED_TEMPERATURE;
}

uint8_t NvsStorage::read_time(const char *key, std::optional<int8_t> &cashed_value) const {
    if (cashed_value) {
        return *cashed_value;
    }

    int8_t result;
    esp_err_t err = nvs_get_i8(nvs_handle_, key, &result);

    std::cout << "Read " << key << " " << +result << std::endl;

    if (err != ESP_OK) {
        return 0;
    }

    cashed_value = result;
    return result;
}

uint8_t NvsStorage::get_start_time_threshold() const noexcept {
    return read_time(START_TIME_KEY, cashed_start_time_threshold_);
}

uint8_t NvsStorage::get_end_time_threshold() const noexcept {
    return read_time(END_TIME_KEY, cashed_end_time_threshold_);
}

bool NvsStorage::write_time(const char *key, int8_t value) noexcept {
    std::cout << "Writing " << key << " " << value << std::endl;

    esp_err_t err = nvs_set_i8(nvs_handle_, key, value);

    if (err != ESP_OK) {
        std::cerr << "Failed to write" << std::endl;
        return false;
    }

    err = nvs_commit(nvs_handle_);

    if (err != ESP_OK) {
        std::cout << "Failed to commit!" << std::endl;
        return false;
    }

    std::cout << "Data saved." << std::endl;
    return true;
}

bool NvsStorage::set_start_time_threshold(int8_t time) noexcept {
    if (!write_time(START_TIME_KEY, time)) {
        return false;
    }

    cashed_start_time_threshold_ = time;
    return true;
}

bool NvsStorage::set_end_time_threshold(int8_t time) noexcept {
    if (!write_time(END_TIME_KEY, time)) {
        return false;
    }

    cashed_end_time_threshold_ = time;
    return true;
}

bool NvsStorage::smart_write(const std::string& key, const std::string &value) noexcept {
    std::cout << "Smart write NVS " << key << ":" << value << std::endl;

    using Callable = std::function<bool(const std::string&, NvsStorage&)>;
    static const std::unordered_map<std::string, Callable> key_function_map{
            {TEMPERATURE_KEY, [](const std::string& value, NvsStorage& storage) {
                return storage.set_temperature_threshold(std::stof(value));
            }},
            {START_TIME_KEY, [](const std::string& value, NvsStorage& storage) {
                return storage.set_start_time_threshold(static_cast<int8_t>(std::stoi(value)));
            }},
            {END_TIME_KEY, [](const std::string& value, NvsStorage& storage) {
                return storage.set_end_time_threshold(static_cast<int8_t>(std::stoi(value)));
            }}
    };

    auto function_it = key_function_map.find(key);

    if (function_it == key_function_map.end()) {
        std::cerr << "Can't find key: " << key << ", val: " << value << std::endl;
        return false;
    }

    function_it->second(value, *this);

    return true;
}

