#ifndef INPUT_SOURCE_H
#define INPUT_SOURCE_H

#include "CharDefinitions.h"

// Abstract base class for raw input
class InputSource {
public:
    virtual ~InputSource() = default;

    virtual TnkChar getChar() = 0;

    virtual TnkChar peekChar() = 0;

    virtual bool eof() = 0;

    virtual bool good() { return true; }
};

#endif