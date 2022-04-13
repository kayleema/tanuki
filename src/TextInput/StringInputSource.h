#ifndef TANUKIPROJECT_STRINGINPUTSOURCE_H
#define TANUKIPROJECT_STRINGINPUTSOURCE_H

#include "InputSource.h"

class StringInputSource : public InputSource {
    const wchar_t *source;
    bool finished;
public:
    explicit StringInputSource(const wchar_t *source) : source(source), finished(false) {};

    ~StringInputSource() override = default;

    wchar_t getChar() override;

    wchar_t peekChar() override;

    bool eof() override;
};

#endif // TANUKIPROJECT_STRINGINPUTSOURCE_H