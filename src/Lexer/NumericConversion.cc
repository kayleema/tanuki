#include "NumericConversion.h"
#include <cmath>
#include "TextInput/UnicodeConversion.h"

static const TnkChar zeroZenkaku = L'０';
static const int zeroZenkakuSize = getUtfCharSize(*"０");
long parseNumeric(const std::string &input_s) {
    auto stringIter = input_s.c_str();
    long result = 0;
    while (*stringIter != 0) {
        auto firstChar = getFirstUtfChar(stringIter);
        long digit = firstChar - zeroZenkaku;
        result = result * 10 + digit;
        stringIter += zeroZenkakuSize;
    }
    return result;
}

static const TnkChar decimalZenkaku = L'。';
static const int decimalZenkakuSize = getUtfCharSize(*"。");
float parseNumericFloat(const std::string& input_s) {
    auto decimalIndex = input_s.find("。");
    auto lhs = input_s.substr(0, decimalIndex);
    auto rhs = input_s.substr(decimalIndex + decimalZenkakuSize);
    auto intPart = parseNumeric(lhs);
    auto fracPart = parseNumeric(rhs);

    auto fracLength = rhs.size() / zeroZenkakuSize;
    auto result = float(intPart) + float(fracPart) / pow(10, fracLength);
    return float(result);
}
