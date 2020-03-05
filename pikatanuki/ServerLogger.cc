#include <Tokenizer.h>
#include "ServerLogger.h"

PinponLogger *ServerLogger::log(std::string value) {
    stringstream out;
    out << R"({)"
        << R"( "messageType" : "display",)"
        << R"( "message" : ")" << value << R"(")"
        << R"(})";
    m_endpoint->send(m_hdl, out.str(), websocketpp::frame::opcode::TEXT);
    return this;
}

PinponLogger *ServerLogger::log(std::wstring value) {
    log(encodeUTF8(value));
    return this;
}
