#ifndef ESP_THERMOSTAT_HTTPSERVER_H
#define ESP_THERMOSTAT_HTTPSERVER_H

#include <string>
#include <esp_http_server.h>
#include <list>

class HttpServer {
public:
    bool init() noexcept;
    void add_handler(httpd_uri_t http_handler) noexcept;

private:
    bool init_wifi() noexcept;
    bool start_server() noexcept;

private:
    httpd_handle_t server_{};
    std::list<httpd_uri_t> handlers_{};

    bool connect();
};


#endif //ESP_THERMOSTAT_HTTPSERVER_H
