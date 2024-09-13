#include "UnicodeConversion.h"
#include "CharDefinitions.h"

using namespace std;

int tnkCharGetSize(TnkChar inputChar) {
    if (inputChar <= 0x007F) {
        return 1; // 1 byte for ASCII
    } else if (inputChar <= 0x07FF) {
        return 2; // 2 bytes for U+0080 to U+07FF
    } else if (inputChar <= 0xFFFF) {
        return 3; // 3 bytes for U+0800 to U+FFFF
    } else if (inputChar <= 0x10FFFF) {
        return 4; // 4 bytes for U+10000 to U+10FFFF
    } else {
        return -1; // Invalid Unicode code point
    }
}

std::string tnkCharToString(TnkChar inputChar) {
    char utf8str[] = {0,0,0,0,0};
    if (inputChar <= 0x7F) {
        // 1-byte UTF-8 (ASCII)
        utf8str[0] = (char)inputChar;
        utf8str[1] = '\0'; // Null terminator
    } else if (inputChar <= 0x7FF) {
        // 2-byte UTF-8
        utf8str[0] = (char)((inputChar >> 6) | 0xC0);  // First byte: 110xxxxx
        utf8str[1] = (char)((inputChar & 0x3F) | 0x80); // Second byte: 10xxxxxx
        utf8str[2] = '\0'; // Null terminator
    } else if (inputChar <= 0xFFFF) {
        // 3-byte UTF-8
        utf8str[0] = (char)((inputChar >> 12) | 0xE0);  // First byte: 1110xxxx
        utf8str[1] = (char)(((inputChar >> 6) & 0x3F) | 0x80); // Second byte: 10xxxxxx
        utf8str[2] = (char)((inputChar & 0x3F) | 0x80); // Third byte: 10xxxxxx
        utf8str[3] = '\0'; // Null terminator
    } else if (inputChar <= 0x10FFFF) {
        // 4-byte UTF-8
        utf8str[0] = (char)((inputChar >> 18) | 0xF0);  // First byte: 11110xxx
        utf8str[1] = (char)(((inputChar >> 12) & 0x3F) | 0x80); // Second byte: 10xxxxxx
        utf8str[2] = (char)(((inputChar >> 6) & 0x3F) | 0x80);  // Third byte: 10xxxxxx
        utf8str[3] = (char)((inputChar & 0x3F) | 0x80);  // Fourth byte: 10xxxxxx
        utf8str[4] = '\0'; // Null terminator
    } else {
        // Invalid code point
    }
    return {utf8str};
}

TnkChar getFirstUtfChar(const char *input) {
    int codepointSize = getUtfCharSize(*input);
    TnkChar codepoint = 0;
    static const TnkChar errorReturnValue = 0;

    if (codepointSize == 1) {
        codepoint = (uint8_t)input[0];
    } else if (codepointSize == 2) {
        if( input[1] == 0 ) {
            return errorReturnValue;
        }
        codepoint = ((uint8_t)input[0] & 0x1F) << 6;
        codepoint |= ((uint8_t)input[1] & 0x3F);
    } else if (codepointSize == 3) {
        if( input[1] == 0 || input[2] == 0 ) {
            return errorReturnValue;
        }
        codepoint = ((uint8_t)input[0] & 0x0F) << 12;
        codepoint |= ((uint8_t)input[1] & 0x3F) << 6;
        codepoint |= ((uint8_t)input[2] & 0x3F);
    } else if (codepointSize == 4) {
        if( input[1] == 0 || input[2] == 0 || input[3] == 0 ) {
            return errorReturnValue;
        }
        codepoint = ((uint8_t)input[0] & 0x07) << 18;
        codepoint |= ((uint8_t)input[1] & 0x3F) << 12;
        codepoint |= ((uint8_t)input[2] & 0x3F) << 6;
        codepoint |= ((uint8_t)input[3] & 0x3F);
    }
    return codepoint;
}

int getUtfCharSize(unsigned char c) {
    if (c < 0x80) {
        return 1; // 1-byte character. (0xxxxxxx) (ASCII)
    } else if ((c & 0xE0) == 0xC0) {
        return 2; // 2-byte character. (110xxxxx 10xxxxxx)
    } else if ((c & 0xF0) == 0xE0) {
        return 3; // 3-byte character. (1110xxxx 10xxxxxx 10xxxxxx)
    } else if ((c & 0xF8) == 0xF0) {
        return 4; // 4-byte character. (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
    }
    return -1; // Invalid UTF-8 character
}

int getCodepointLength(const char *input){
    int length = 0;
    while (*input != 0) {
        input += getUtfCharSize(*input);
        length++;
    }
    return length;
}

