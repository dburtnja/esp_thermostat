#include <format>
#include "View.h"

View::View(const NvsStorage &nvs_storage, const Thermometer &thermometer, const IpClock &clock, const Heater& heater)
    : nvs_storage_(nvs_storage), thermometer_(thermometer), clock_(clock), heater_(heater) {
}

std::string View::get_json_data() const noexcept {
    return std::format(iot_json_template, thermometer_.get_value());
}

std::string View::get_home_page(const std::string& status) const noexcept {
    // Temperature threshold, start threshold, end threshold, current temperature
    auto result = std::format(main_paige_html_template,
                              thermometer_.get_value(),
                              nvs_storage_.get_temperature_threshold(),
                              nvs_storage_.get_start_time_threshold(),
                              nvs_storage_.get_end_time_threshold(),
                              latest_check_time_,
                              heater_.to_string(),
                              status);

    return result;
}

void View::save_latest_check_time() {
    latest_check_time_ = clock_.to_string();
}

