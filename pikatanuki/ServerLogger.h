#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#define ASIO_STANDALONE

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "Logger.h"

typedef websocketpp::server<websocketpp::config::asio> server;

class ServerLogger : public PinponLogger {
    server *m_endpoint;
    websocketpp::connection_hdl m_hdl;
public:
    ServerLogger(server *endpoint, websocketpp::connection_hdl hdl) : m_endpoint(endpoint), m_hdl(std::move(hdl)) {}
    PinponLogger *log(std::string value) override;
    PinponLogger *log(std::wstring value) override;
    PinponLogger *logEndl() override { return this; };
    PinponLogger *setup() override { return this; };
};

#endif //SERVER_LOGGER_H
