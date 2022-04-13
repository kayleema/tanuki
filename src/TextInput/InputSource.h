#ifndef INPUT_SOURCE_H
#define INPUT_SOURCE_H

// Abstract base class for raw input
class InputSource {
public:
    virtual ~InputSource() = default;

    virtual wchar_t getChar() = 0;

    virtual wchar_t peekChar() = 0;

    virtual bool eof() = 0;

    virtual bool good() { return true; }
};

#endif