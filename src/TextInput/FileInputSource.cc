#include "FileInputSource.h"
#include "StringInputSource.h"
#include <cstdint>

using namespace std;

FileInputSource::FileInputSource(const char *filename) : file(filename) {
    if (file.bad() || !file.good()) {
        //cout << "ERROR：ファイルを開けません。[" << filename << "]" << endl;
    }
}

TnkChar FileInputSource::getChar() {
    if (!peeked) {
        peekChar();
    }
    peeked = false;
    return peekedChar;
}

TnkChar FileInputSource::peekChar() {
    if (peeked) {
        return peekedChar;
    }
    peeked = true;
    if (eof()) {
        peekedChar = 0;
        return 0;
    }
    int codepointSize = getUtfCharSize(file.peek());
    char readBuffer[4] = {0};
    file.read(readBuffer, codepointSize);

    uint32_t codepoint = 0;
    if (codepointSize == 1) {
        codepoint = (uint8_t)readBuffer[0]; // 1-byte ASCII character
    } else if (codepointSize == 2) {
        codepoint = ((uint8_t)readBuffer[0] & 0x1F) << 6;
        codepoint |= ((uint8_t)readBuffer[1] & 0x3F);
    } else if (codepointSize == 3) {
        codepoint = ((uint8_t)readBuffer[0] & 0x0F) << 12;
        codepoint |= ((uint8_t)readBuffer[1] & 0x3F) << 6;
        codepoint |= ((uint8_t)readBuffer[2] & 0x3F);
    } else if (codepointSize == 4) {
        codepoint = ((uint8_t)readBuffer[0] & 0x07) << 18;
        codepoint |= ((uint8_t)readBuffer[1] & 0x3F) << 12;
        codepoint |= ((uint8_t)readBuffer[2] & 0x3F) << 6;
        codepoint |= ((uint8_t)readBuffer[3] & 0x3F);
    }

    peekedChar = (TnkChar)codepoint;
    return (TnkChar)codepoint;
}

bool FileInputSource::eof() {
    return file.eof();
}
