#define ASIO_STANDALONE

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>
#include <iostream>
#include <set>
#include <signal.h>
#include "Context.h"
#include "CoreFunctions.h"

typedef websocketpp::server<websocketpp::config::asio> server;

void evalPinponStarter(Environment *env);

class utility_server {
public:
    Context *context;
    Environment *environment;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connectionSet;

    utility_server() {
        // Set logging settings
        m_endpoint.set_error_channels(websocketpp::log::elevel::all);
        m_endpoint.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
        m_endpoint.set_reuse_addr(true);

        // Initialize Asio
        m_endpoint.init_asio();

        // Set the default message handler to the echo handler
        m_endpoint.set_message_handler(std::bind(
                &utility_server::echo_handler, this,
                std::placeholders::_1, std::placeholders::_2
        ));

        m_endpoint.set_open_handler(std::bind(
                &utility_server::open_handler, this,
                std::placeholders::_1
        ));

        m_endpoint.set_close_handler(std::bind(
                &utility_server::close_handler, this,
                std::placeholders::_1
        ));

        context = new Context();
        context->setFrequency(10);
        environment = new Environment(context);
        evalPinponStarter(environment);
    }

    void echo_handler(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        m_endpoint.send(hdl, msg->get_payload(), msg->get_opcode());

        string inputRaw = msg->get_payload();
        wstring input = decodeUTF8(inputRaw);

        ConsoleLogger log;
        auto source = StringInputSource(input.c_str());
        auto tokenizer = InputSourceTokenizer(&source);
        auto parser = Parser(&tokenizer, &log);
        SyntaxNode *tree = parser.run();
        auto result = environment->eval(tree->children[0]);

        m_endpoint.send(hdl, std::string("》") + result->toString(), msg->get_opcode());
    }

    void open_handler(websocketpp::connection_hdl hdl) {
        connectionSet.insert(hdl);
    }

    void close_handler(websocketpp::connection_hdl hdl) {
        std::cout << "closehandled" << std::endl;
        connectionSet.erase(hdl);
    }

    void run() {
        // Listen on port 9002
        m_endpoint.listen(9002);

        // Queues a connection accept operation
        m_endpoint.start_accept();

        std::cout << "server started" << std::endl;
        // Start the Asio io_service run loop
        m_endpoint.run();
    }

    void stop() {
        std::cout << "closing active connections..." << std::endl;
        m_endpoint.stop_listening();
        for (auto c : connectionSet) {
            std::cout << "close..." << std::endl;
            m_endpoint.close(c, websocketpp::close::status::going_away, "bye");
        }
        std::cout << "closed" << std::endl;
    }

private:
    server m_endpoint;
};

utility_server *running_server = nullptr;

void shutdownInterrupt(int) {
    running_server->stop();
}

int main() {
    utility_server s;
    running_server = &s;
    signal(SIGINT, shutdownInterrupt);
    signal(SIGTERM, shutdownInterrupt);
    s.run();
    return 0;
}


void evalPinponStarter(Environment *env) {
    ConsoleLogger log;
    auto source = StringInputSource(corePinponStarter);
    auto tokenizer = InputSourceTokenizer(&source);
    auto parser = Parser(&tokenizer, &log);
    SyntaxNode *tree = parser.run();
    env->eval(tree);
}
