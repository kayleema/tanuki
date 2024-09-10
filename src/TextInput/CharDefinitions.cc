#include "CharDefinitions.h"

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
