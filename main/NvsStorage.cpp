#include "NvsStorage.h"

#include "nvs_flash.h"

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

    // todo validate existence of the data

    return true;
}

std::optional<float> NvsStorage::get_temperature_threshold() noexcept {
    size_t size{};
    float result;

    esp_err_t err = nvs_get_blob(nvs_handle_, TEMPERATURE_KEY,
                                 &result, &size);
    if (err == ESP_OK && size == sizeof(float)) {
        return result;
    } else {
        printf("No temperature.");
        return {};
    }
}

