#ifndef ESP_THERMOSTAT_VIEW_H
#define ESP_THERMOSTAT_VIEW_H

#include "NvsStorage.h"
#include "Thermometer.h"
#include "IpClock.h"
#include "Heater.h"

#include <string>

class View {
public:
    explicit View(const NvsStorage &nvs_storage,
                  const Thermometer &thermometer,
                  const IpClock &clock,
                  const Heater& heater);

    [[nodiscard]] std::string get_home_page(const std::string& status) const noexcept;
    void save_latest_check_time();
    [[nodiscard]] std::string get_json_data() const noexcept;

private:
    const NvsStorage& nvs_storage_;
    const Thermometer& thermometer_;
    const IpClock& clock_;
    const Heater& heater_;

    std::string latest_check_time_{};

    static constexpr const char* iot_json_template = R"rawliteral(
{{"temperature":{}}}
)rawliteral";

    static constexpr const char* main_paige_html_template = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Thermostat</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            background-color: #f4f4f9;
            text-align: center;
        }}
        h1 {{
            font-size: 2rem;
            margin-bottom: 20px;
        }}
        p, label {{
            font-size: 1rem;
            margin: 10px 0;
        }}
        input {{
            width: 90%;
            max-width: 300px;
            padding: 10px;
            margin: 10px 0;
            font-size: 1rem;
        }}
        button {{
            padding: 10px 20px;
            font-size: 1rem;
            background-color: #007bff;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }}
        button:hover {{
            background-color: #0056b3;
        }}
    </style>
</head>
<body>
    <h1>ESP32 Thermostat</h1>
    <p>Temperature: {}</p>
    <form method="post" action="/">
        <label for="temperature">Enter high temperature, (current {}):</label>
        <input type="text" id="temperature" name="temperature">
        <br/>
        <label for="start_time">Enter time, start (current {}):</label>
        <input type="text" id="start_time" name="start_time">
        <br/>
        <label for="end_time">Enter time, end (current {}):</label>
        <input type="text" id="end_time" name="end_time">
        <br/>
        <button type="submit">Submit</button>
    </form>
    <p>Latest check time: {}</p>
    <p>Heater: {} Status: {}</p>
    <br/>
    <button onclick="window.location.href='/'">Reload</button>
</body>
</html>
)rawliteral";

};


#endif //ESP_THERMOSTAT_VIEW_H
