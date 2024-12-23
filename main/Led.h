#ifndef ESP_THERMOSTAT_LED_H
#define ESP_THERMOSTAT_LED_H

#include <thread>

class Led{
public:
    void init() noexcept;

    void fast_blinking_blocking_call_1_min() noexcept;
    void on() noexcept;
    void off() noexcept;

    void slow_blinking();
    void stop_slow_blinking();

private:
    std::thread task_{};
    bool is_on{false};
    std::atomic<bool> is_blinking{false};

    static void blink_interval_ms(uint32_t interval);
};


#endif //ESP_THERMOSTAT_LED_H
