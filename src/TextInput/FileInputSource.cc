#include <codecvt>
#include <iostream>
#include "FileInputSource.h"

using namespace std;

FileInputSource::FileInputSource(const char *filename) : file(filename) {
    file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    if (file.bad() || !file.good()) {
        cout << "ERROR：ファイルを開けません。[" << filename << "]" << endl;
    }
}

wchar_t FileInputSource::getChar() {
    wchar_t first;
    file.get(first);
    if (eof()) {
        return L'\0';
    }
    return first;
}

wchar_t FileInputSource::peekChar() {
    wchar_t result = file.peek();
    if (eof()) {
        return L'\0';
    }
    return result;
}

bool FileInputSource::eof() {
    return file.eof();
}
