#include "TanukiServerREPL.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TanukiServerREPL::TanukiServerREPL() {
    m_endpoint.set_error_channels(websocketpp::log::elevel::all);
    m_endpoint.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
    m_endpoint.set_reuse_addr(true);

    m_endpoint.init_asio();

    m_endpoint.set_message_handler(std::bind(
            &TanukiServerREPL::handleMessage, this,
            std::placeholders::_1, std::placeholders::_2
    ));

    m_endpoint.set_open_handler(std::bind(
            &TanukiServerREPL::handleOpen, this,
            std::placeholders::_1
    ));

    m_endpoint.set_close_handler(std::bind(
            &TanukiServerREPL::handleClose, this,
            std::placeholders::_1
    ));

}

void TanukiServerREPL::handleMessage(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    string inputRaw = msg->get_payload();
    auto parsed = json::parse(inputRaw, nullptr, false);
    if (parsed == json::value_t::discarded) {
        return;
    } else if (parsed["dummy"] == "clear"){
        return;
    } else if (parsed["messageType"] == "clear"){
        return;
    } else if (parsed["messageType"] == "code" | parsed["messageType"] == "file"){
        inputRaw = parsed["code"];
    }
    bool oneLine = true;
    if (parsed["messageType"] == "file") {
        oneLine = false;
    }
    wstring input = decodeUTF8(inputRaw);

    ConsoleLogger log;
    auto source = StringInputSource(input.c_str());
    auto tokenizer = InputSourceTokenizer(&source);
    auto parser = Parser(&tokenizer, &log);
    SyntaxNode *tree = parser.run();
    if (oneLine) {
        tree = tree->children[0];
    }
    auto result = environments[hdl]->eval(tree);

    stringstream out;
    out << R"({)"
        << R"( "messageType" : "result",)"
        << R"( "resultString" : ")" << result->toStringJP() << R"(")"
        << R"(})";
    m_endpoint.send(hdl, out.str(), msg->get_opcode());

    delete tree;
}
