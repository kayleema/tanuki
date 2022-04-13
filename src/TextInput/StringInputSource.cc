#include "TextInput/StringInputSource.h"

wchar_t StringInputSource::getChar() {
    if (*source == L'\0') {
        finished = true;
        return L'\0';
    }
    return *source++;
}
wchar_t StringInputSource::peekChar() {
    return *source;
}
bool StringInputSource::eof() {
    return finished;
}