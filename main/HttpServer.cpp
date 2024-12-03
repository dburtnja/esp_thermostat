#include "HttpServer.h"

#include <iostream>
#include <esp_log.h>
#include <cstring>
#include "esp_wifi.h"
#include "esp_http_server.h"

#include <esp_mac.h>
#include <vector>


#define DEFAULT_SSID CONFIG_WIFI_SSID
#define DEFAULT_PWD CONFIG_WIFI_PASSWORD
#define SECOND_SSID CONFIG_SECONDARY_WIFI_SSID
#define SECOND_PWD CONFIG_SECONDARY_WIFI_PASSWORD
#define MAX_RETRIES CONFIG_WIFI_MAX_RETRIES

bool HttpServer::init() noexcept {
    if (!init_wifi()) {
        return false;
    }

    if (!start_server()) {
        return false;
    }

    return true;
}

bool HttpServer::init_wifi() noexcept {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    auto wifi_events_handler = [] (void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) -> void {
        auto* this_server = static_cast<HttpServer *>(arg);

        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
            if (this_server->connect()) {
                ESP_LOGI(__FILENAME__, "Couldn't connect...");
            }
        } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
            if (this_server->connect()) {
                ESP_LOGI(__FILENAME__, "Wi-Fi disconnected, reconnecting...");
            }
        }
    };

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        wifi_events_handler, this, nullptr));

    auto ip_events_handler = [] (void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) -> void {
        if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            auto *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(__FILENAME__, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        }
    };

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                        ip_events_handler, nullptr, nullptr));

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();

    if (!sta_netif) {
        return false;
    }

    return connect();
}

bool HttpServer::connect() {
    std::vector<std::pair<std::string, std::string>> credentials{
            {DEFAULT_SSID, DEFAULT_PWD},
            {SECOND_SSID, SECOND_PWD}
    };

    for (const auto& [ssid, pwd] : credentials) {
        ESP_LOGI(__FILENAME__, "Connecting to SSID: %s...", ssid.c_str());
        wifi_config_t wifi_config{};
        ssid.copy(reinterpret_cast<char*>(wifi_config.sta.ssid), sizeof(wifi_config.sta.ssid));
        pwd.copy(reinterpret_cast<char*>(wifi_config.sta.password), sizeof(wifi_config.sta.password));

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        // Wait for connection event or timeout
        for (int retry{}; retry < MAX_RETRIES; ++retry) {
            ESP_LOGI(__FILENAME__, "Connecting to SSID: %s. Try %d\n.", ssid.c_str(), retry);
            if (esp_wifi_connect() == ESP_OK) {
                ESP_LOGI(__FILENAME__, "Connected");
                return true;
            } else {
                ESP_LOGI(__FILENAME__, "Waiting 1sec.");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }

    return false;
}

bool HttpServer::start_server() noexcept {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(__FILENAME__, "Starting HTTP server...");
    if (httpd_start(&server_, &config) != ESP_OK) {
        ESP_LOGE(__FILENAME__, "Failed to start HTTP server");
        return false;
    }

    return true;
}

void HttpServer::add_handler(httpd_uri_t http_handler) noexcept {
    auto& saved_handler = handlers_.emplace_back(http_handler);
    httpd_register_uri_handler(server_, &saved_handler);
}
