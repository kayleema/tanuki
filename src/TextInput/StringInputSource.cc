#include "TextInput/StringInputSource.h"
#include "UnicodeConversion.h"
#include <cstdint>

using std::uint8_t;

TnkChar StringInputSource::getChar() {
    TnkChar codepoint = peekChar();
    char currentChar = *source;
    int codepointSize = getUtfCharSize(currentChar);
    if (codepoint == 0) {
        finished = true;
    } else {
        source += codepointSize;
    }
    return codepoint;
}
TnkChar StringInputSource::peekChar() {
    char currentChar = *source;
    int codepointSize = getUtfCharSize(currentChar);

    uint32_t codepoint = 0;

    if (codepointSize == 1) {
        codepoint = (uint8_t)source[0]; // 1-byte ASCII character
    } else if (codepointSize == 2) {
        codepoint = ((uint8_t)source[0] & 0x1F) << 6;
        codepoint |= ((uint8_t)source[1] & 0x3F);
    } else if (codepointSize == 3) {
        codepoint = ((uint8_t)source[0] & 0x0F) << 12;
        codepoint |= ((uint8_t)source[1] & 0x3F) << 6;
        codepoint |= ((uint8_t)source[2] & 0x3F);
    } else if (codepointSize == 4) {
        codepoint = ((uint8_t)source[0] & 0x07) << 18;
        codepoint |= ((uint8_t)source[1] & 0x3F) << 12;
        codepoint |= ((uint8_t)source[2] & 0x3F) << 6;
        codepoint |= ((uint8_t)source[3] & 0x3F);
    }

    if (codepoint == 0) {
        return 0;
    }
    return (TnkChar)codepoint;
}
bool StringInputSource::eof() {
    return finished;
}