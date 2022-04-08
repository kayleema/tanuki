#include "NumericConversion.h"
#include <math.h>

long parseNumeric(std::wstring s) {
    if (s.length() == 0) {
        return 0;
    }
    long result = s.back() - L'０';
    s.pop_back();
    return result + parseNumeric(s) * 10;
}

float parseNumericFloat(std::wstring s) {
    auto position = s.find(L'。');
    auto lhs = s.substr(0, position);
    auto rhs = s.substr(position + 1);
    auto intPart = parseNumeric(lhs);
    auto fracPart = parseNumeric(rhs);
    auto fracLength = rhs.size();
    auto result = float(intPart) + float(fracPart) / pow(10, fracLength);
    return result;
}
