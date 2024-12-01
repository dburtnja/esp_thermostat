#ifndef ESP_THERMOSTAT_LED_H
#define ESP_THERMOSTAT_LED_H

#include <thread>

class Led{
public:
    void init() noexcept;

    void fast_blinking() noexcept;
    void on() noexcept;
    void off() noexcept;

private:
    std::thread task{};
};


#endif //ESP_THERMOSTAT_LED_H
