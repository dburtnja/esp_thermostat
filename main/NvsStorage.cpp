#include "NvsStorage.h"

#include "nvs_flash.h"

#define DEFAULT_TEMPERATURE_THRESHOLD 21.5f

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

    auto is_ok = get_temperature_threshold();

    if (!is_ok) {
        // Setting default values
        if (!set_temperature_threshold(DEFAULT_TEMPERATURE_THRESHOLD)) {
            return false;
        }
    }

    return true;
}

bool NvsStorage::set_temperature_threshold(float threshold) noexcept {
    printf("Writing date to NVS...\n");

    esp_err_t err = nvs_set_blob(nvs_handle_, TEMPERATURE_KEY, &threshold, sizeof(threshold));
    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle_);
        if (err == ESP_OK) {
            printf("Date saved.");
            return true;
        } else {
            printf("Failed to commit date!\n");
            return false;
        }
    } else {
        printf("Failed to save date!\n");
        return false;
    }
}

std::optional<float> NvsStorage::get_temperature_threshold() const noexcept {
    if (cashed_temperature_threshold) {
        return cashed_temperature_threshold;
    }

    size_t size{sizeof(float)};
    float result;

    esp_err_t err = nvs_get_blob(nvs_handle_, TEMPERATURE_KEY,
                                 &result, &size);

    printf("Read t=%f, data_size=%ui", result, size);
    if (err == ESP_OK) {
        cashed_temperature_threshold = result;
        return result;
    } else {
        printf("No temperature.");
        return {};
    }
}

