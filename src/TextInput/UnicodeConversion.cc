#include "UnicodeConversion.h"
#include <codecvt>
#include <iostream>
#include <locale>

using namespace std;

string encodeUTF8(const wstring &in) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
        return conv1.to_bytes(in);
    } catch (const std::range_error &exception) {
        cout << "could not convert to utf8 string " << in.size() << endl;
        return {"XXX"};
    }
}

wstring decodeUTF8(const string &in) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
        return conv1.from_bytes(in);
    } catch (const std::range_error &exception) {
        return {L"XXX"};
    }
}
