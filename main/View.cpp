#include <format>
#include "View.h"

View::View(const NvsStorage &nvs_storage, const Thermometer& thermometer, const IpClock& clock)
    : nvs_storage_(nvs_storage), thermometer_(thermometer), clock_(clock) {
}

std::string View::get_home_page(const std::string& status) const noexcept {
    // Temperature threshold, start threshold, end threshold, current temperature
    auto result = std::format(main_paige_html_template,
                              thermometer_.get_value(),
                              nvs_storage_.get_temperature_threshold(),
                              nvs_storage_.get_start_time_threshold(),
                              nvs_storage_.get_end_time_threshold(),
                              clock_.to_string(),
                              status);

    return result;
}

