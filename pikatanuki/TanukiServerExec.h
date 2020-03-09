#ifndef TANUKI_SERVER_H
#define TANUKI_SERVER_H

#define ASIO_STANDALONE

#include <set>
#include <functional>
#include <iostream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <utility>

#include "Context.h"
#include "ServerLogger.h"
#include "starterEvaluator.h"

typedef websocketpp::server<websocketpp::config::asio> server;

class TanukiServerExec {
public:
    Context *context;
    Environment *environment;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connectionSet;
    std::vector<ServerLogger *> serverLoggers;

    TanukiServerExec();

    void handleMessage(websocketpp::connection_hdl hdl, server::message_ptr msg);

    void handleOpen(websocketpp::connection_hdl hdl) {
        auto *connectionLogger = new ServerLogger(&m_endpoint, hdl);
        serverLoggers.push_back(connectionLogger);
        environment->logger = connectionLogger;

        connectionSet.insert(hdl);
    }

    void handleClose(websocketpp::connection_hdl hdl) {
        connectionSet.erase(hdl);
    }

    void run() {
        m_endpoint.listen(9002);

        m_endpoint.start_accept();

        std::cout << "server started" << std::endl;
        m_endpoint.run();
    }

    void stop() {
        std::cout << "closing active connections..." << std::endl;
        m_endpoint.stop_listening();
        for (auto c : connectionSet) {
            m_endpoint.close(c, websocketpp::close::status::going_away, "bye");
        }
        std::cout << "...closed" << std::endl;
    }

private:
    server m_endpoint;
};

#endif