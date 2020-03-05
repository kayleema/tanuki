#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class PinponLogger {
public:
    virtual ~PinponLogger() = default;

    virtual PinponLogger *log(std::string value) = 0;

    virtual PinponLogger *log(std::wstring value) = 0;

    virtual PinponLogger *logLong(long value);

    virtual PinponLogger *logLn(std::string value);

    virtual PinponLogger *logLn(std::wstring value);

    virtual PinponLogger *logEndl() = 0;

    virtual PinponLogger *setup() = 0;
};

class ConsoleLogger : public PinponLogger {
public:
    static bool wide_mode;

    PinponLogger *log(std::string value) override;

    PinponLogger *log(std::wstring value) override;

    PinponLogger *logLn(std::string value) override;

    PinponLogger *logLn(std::wstring value) override;

    PinponLogger *logEndl() override;

    PinponLogger *setup() override;
};

#endif //LOGGER_H
