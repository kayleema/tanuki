#include "TanukiServer.h"

TanukiServer::TanukiServer() {
    m_endpoint.set_error_channels(websocketpp::log::elevel::all);
    m_endpoint.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
    m_endpoint.set_reuse_addr(true);

    m_endpoint.init_asio();

    m_endpoint.set_message_handler(std::bind(
            &TanukiServer::handleMessage, this,
            std::placeholders::_1, std::placeholders::_2
    ));

    m_endpoint.set_open_handler(std::bind(
            &TanukiServer::handleOpen, this,
            std::placeholders::_1
    ));

    m_endpoint.set_close_handler(std::bind(
            &TanukiServer::handleClose, this,
            std::placeholders::_1
    ));

    context = new Context();
    context->setFrequency(10);
    environment = new Environment(context);
    evalPinponStarter(environment);
}

void TanukiServer::handleMessage(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    string inputRaw = msg->get_payload();
    wstring input = decodeUTF8(inputRaw);

    ConsoleLogger log;
    auto source = StringInputSource(input.c_str());
    auto tokenizer = InputSourceTokenizer(&source);
    auto parser = Parser(&tokenizer, &log);
    SyntaxNode *tree = parser.run();
    auto result = environment->eval(tree->children[0]);

    stringstream out;
    out << R"({)"
        << R"( "messageType" : "result",)"
        << R"( "resultString" : ")" << result->toString() << R"(")"
        << R"(})";
    m_endpoint.send(hdl, out.str(), msg->get_opcode());
}
