#ifndef TANUKIPROJECT_STRINGINPUTSOURCE_H
#define TANUKIPROJECT_STRINGINPUTSOURCE_H

#include "InputSource.h"
#include "CharDefinitions.h"
#include "UnicodeConversion.h"

class StringInputSource : public InputSource {
    const char *source;
    bool finished;
public:
    explicit StringInputSource(const char *source) : source(source), finished(false) {};

    ~StringInputSource() override = default;

    TnkChar getChar() override;

    TnkChar peekChar() override;

    bool eof() override;
};

#endif // TANUKIPROJECT_STRINGINPUTSOURCE_H