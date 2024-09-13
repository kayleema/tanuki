#include "Token.h"
#include "NumericConversion.h"
#include <sstream>

Token::Token(TokenType type, string _content, int line)
    : type(type), content(std::move(_content)), line(line) {
    if (type == TokenType::NUMBER) {
        number = parseNumeric(content);
    }
    if (type == TokenType::NUMBER_FLOAT) {
        numberFloat = parseNumericFloat(content);
    }
}

string Token::toString() const {
    ostringstream result("");
    result << TokenTypeStrings[(int)type];
    result << "：”" << content;
    if (type == TokenType::NUMBER) {
        result << "（" << number << "）";
    }
    if (type == TokenType::NUMBER_FLOAT) {
        result << "（" << numberFloat << "）";
    }
    result << "”、" << line << "行目";
    return result.str();
}

bool Token::operator==(const Token &rhs) const {
    return type == rhs.type && content == rhs.content && line == rhs.line;
}

ostream &operator<<(ostream &os, const Token &token) {
    return os << "Token(" << TokenTypeStrings[(int)token.type] << ", \""
              << token.content << "\", " << token.line << ")";
}

const char *tokenTypeToString(TokenType type) {
    return TokenTypeStrings[(int)type];
}

string tokenListToTypeList(vector<Token> list) {
    string result;
    bool first = true;
    for (auto &token : list) {
        if (!first) {
            result.append(", ");
        }
        result.append(tokenTypeToString(token.type));
        first = false;
    }
    return result;
}
