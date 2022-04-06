#include "Logger.h"
#include "Lexer/Tokenizer.h"
#include <iostream>


#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

PinponLogger *ConsoleLogger::setup() {
    if (wide_mode) {
#ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
#endif
    }
    return this;
}

PinponLogger *ConsoleLogger::log(std::string value) {
    if (wide_mode) {
        std::wcout << decodeUTF8(value);
    } else {
        std::cout << value;
    }
    return this;
}

PinponLogger *ConsoleLogger::log(std::wstring value) {
    if (wide_mode) {
        std::wcout << value;
    } else {
        std::cout << encodeUTF8(value);
    }
    return this;
}

PinponLogger *ConsoleLogger::logLn(std::string value) {
    log(value);
    logEndl();
    return this;
}

PinponLogger *ConsoleLogger::logLn(std::wstring value) {
    log(value);
    logEndl();
    return this;
}

PinponLogger *ConsoleLogger::logEndl() {
    if (wide_mode) {
        std::wcout << std::endl;
    } else {
        std::cout << std::endl;
    }
    return this;
}

PinponLogger *PinponLogger::logLong(long value) {
    stringstream ss;
    ss << value;
    log(ss.str());
    return this;
}

PinponLogger *PinponLogger::logLn(std::string value) {
    log(value);
    logEndl();
    return this;
}

PinponLogger *PinponLogger::logLn(std::wstring value) {
    log(value);
    logEndl();
    return this;
}

#ifdef _WIN32
bool ConsoleLogger::wide_mode = true;
#else
bool ConsoleLogger::wide_mode = false;
#endif
