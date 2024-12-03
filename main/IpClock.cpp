#include <ctime>
#include <esp_sntp.h>
#include <iostream>
#include <vector>
#include "IpClock.h"

#define TIME_ZONE "UTC-2"
#define SERVER_NAME "pool.ntp.org"

auto is_time_synced = [] (struct tm timeinfo) {
    std::cerr << "year " << timeinfo.tm_year;
    return timeinfo.tm_year != 0;
};

bool IpClock::init() noexcept {
    setenv("TZ", TIME_ZONE, 1);
    tzset();

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, SERVER_NAME);

    esp_sntp_set_time_sync_notification_cb(
            [](struct timeval* tv){ std::cout << "Time synchronized" << std::endl; });
    esp_sntp_init();

    return true;
}

int8_t IpClock::get_current_hour() const noexcept {
    const int retries{10};
    struct tm timeinfo{};
    time_t now{};

    time(&now);
    localtime_r(&now, &timeinfo);

    for (int i{}; i < retries; ++i) {
        if (is_time_synced(timeinfo)) {
            break;
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    return static_cast<int8_t>(timeinfo.tm_hour);
}

std::string IpClock::to_string() const noexcept {
    struct tm timeinfo{};
    time_t now{};

    time(&now);
    localtime_r(&now, &timeinfo);

    std::vector<char> buff(64);
    if (strftime(buff.data(), buff.size(), "%Y-%m-%d %H:%M:%S", &timeinfo) == 0) {
        return "Can't get time";
    }

    return buff.data();
}
