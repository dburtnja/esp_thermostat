#include <iostream>
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <esp_log.h>
#include <format>
#include <vector>
#include <sstream>

#include "NvsStorage.h"
#include "Led.h"
#include "Thermometer.h"
#include "HttpServer.h"
#include "View.h"
#include "IpClock.h"
#include "Heater.h"

void add_post_handlers(HttpServer& server, std::pair<View*,NvsStorage*>& context) {
    auto accept_data = [] (httpd_req_t *req) -> esp_err_t {
        if (!req->user_ctx) {
            return ESP_FAIL;
        }

        auto* context = static_cast<std::pair<View *, NvsStorage *> *>(req->user_ctx);

        std::vector<char> buffer(512);
        httpd_req_recv(req, buffer.data(), buffer.size());

        std::cout << "Request: " << buffer.data() << std::endl;

        esp_err_t result{ESP_OK};
        std::stringstream stream{buffer.data()};
        for (std::string buf; std::getline(stream, buf, '&');) {
            auto delimiter_pos = buf.find('=');
            if (delimiter_pos == std::string::npos) {
                continue;
            }

            std::string value = buf.substr(delimiter_pos + 1);
            if (value.empty()) {
                continue;
            }
            std::string key = buf.substr(0, delimiter_pos);

            std::cerr << "Smart write started" << std::endl;
            if (!context->second->smart_write(key, value)) {
                result = ESP_FAIL;
                break;
            }
        }

        std::string response{};
        if (result == ESP_OK) {
            response = context->first->get_home_page("Updated.");
        } else {
            response = context->first->get_home_page("Unexpected input!!!!");
        }

        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return result;
    };


    httpd_uri_t handle{
        .uri = "/",
        .method = HTTP_POST,
        .handler = accept_data,
        .user_ctx = &context
    };
    server.add_handler(handle);
}

void add_get_handler(HttpServer& server, const View& view) {
    auto main_page_handler = [] (httpd_req_t *req) -> esp_err_t {
        auto* view = (View*)req->user_ctx; // todo add RTTI

        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, view->get_home_page("OK").c_str(), HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    };

    // Register a handler for the "/" URI
    httpd_uri_t main_page = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = main_page_handler,
            .user_ctx = (void*)&view
    };

    server.add_handler(main_page);
}

extern "C" void app_main(void)
{
    NvsStorage nvs_storage{};
    Led led{};
    Thermometer thermometer{};
    HttpServer server{};
    IpClock clock{};
    Heater heater{};
    View view{nvs_storage, thermometer, clock, heater};
    std::pair<View*,NvsStorage*> pointers{&view, &nvs_storage};

    led.init();
    led.slow_blinking();

    heater.init();
    heater.set(OFF);

    if (!nvs_storage.init()) {  // Should be initialised first!
        ESP_LOGI(__FILENAME__, "Error: can't initialise NVS.\n");
        led.fast_blinking_blocking_call_1_min();
        abort();
    }

    if (!thermometer.init()) {
        ESP_LOGI(__FILENAME__, "Error: can't initialise thermometer.\n");
        led.fast_blinking_blocking_call_1_min();
        abort();
    }

    if (!server.init()) {
        ESP_LOGI(__FILENAME__, "Error: can't initialise Server.\n");
        led.fast_blinking_blocking_call_1_min();
        abort();
    }

    if (!clock.init()) {
        ESP_LOGI(__FILENAME__, "Error: can't initialise IpClock.\n");
        led.fast_blinking_blocking_call_1_min();
        abort();
    }

    add_get_handler(server, view);
    add_post_handlers(server, pointers);

    led.stop_slow_blinking();
    led.on();

    while (true) {
        auto current_hour = clock.get_current_hour();
        if (current_hour < nvs_storage.get_start_time_threshold() ||
            current_hour > nvs_storage.get_end_time_threshold()) {
            heater.set(OFF);
            vTaskDelay(pdMS_TO_TICKS(60000));
            continue;
        }

        auto temperature = thermometer.get_value();
        auto temperature_threshold = nvs_storage.get_temperature_threshold();

        std::cout << "Current temperature: " << temperature << std::endl;
        std::cout << "Temperature threshold: " << temperature_threshold << std::endl;

        if (temperature < temperature_threshold) {
            heater.set(ON);
        } else {
            heater.set(OFF);
        }

        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
