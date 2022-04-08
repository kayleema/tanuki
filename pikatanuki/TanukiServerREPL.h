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

class TanukiServerREPL {
public:
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connectionSet;
    std::map<websocketpp::connection_hdl, Environment*, std::owner_less<websocketpp::connection_hdl>> environments;

    TanukiServerREPL();

    void handleMessage(websocketpp::connection_hdl hdl, server::message_ptr msg);

    void handleOpen(websocketpp::connection_hdl hdl) {
        cout << m_endpoint.get_con_from_hdl(hdl)->get_request_header("Cookie") << endl;
        auto *connectionLogger = new ServerLogger(&m_endpoint, hdl);
        environments[hdl] = new Environment(new Context(), nullptr, connectionLogger);
        environments[hdl]->context->setFrequency(10);
        environments[hdl]->exitHandler = connectionLogger;
        evalTanukiStarter(environments[hdl]);

        connectionSet.insert(hdl);
    }

    bool handleValidate(websocketpp::connection_hdl hdl) {
        cout << "VALIDATE" << endl;
        cout << m_endpoint.get_con_from_hdl(hdl)->get_request_header("Cookie") << endl;
        return true;
    }

    void handleClose(websocketpp::connection_hdl hdl) {
        auto env = environments[hdl];
        auto con = env->context;
        auto log = env->logger;
        con->cleanup();
        delete con;
        delete log;
        environments.erase(hdl);
        connectionSet.erase(hdl);
    }

    void handleHttp(websocketpp::connection_hdl hdl) {
        auto con = m_endpoint.get_con_from_hdl(hdl);

        con->set_body("Healthy");
        con->set_status(websocketpp::http::status_code::ok);
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