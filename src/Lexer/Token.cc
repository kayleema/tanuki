#include "Token.h"
#include "Tokenizer.h"
#include "NumericConversion.h"

Token::Token(TokenType type, wstring _content, int line)
    : type(type), content(move(_content)), line(line) {
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
    result << u8"：”" << encodeUTF8(content);
    if (type == TokenType::NUMBER) {
        result << u8"（" << number << u8"）";
    }
    if (type == TokenType::NUMBER_FLOAT) {
        result << u8"（" << numberFloat << u8"）";
    }
    result << u8"”、" << line << u8"行目";
    return result.str();
}

bool Token::operator==(const Token &rhs) const {
    return type == rhs.type && content == rhs.content && line == rhs.line;
}

ostream &operator<<(ostream &os, const Token &token) {
    return os << "Token(" << TokenTypeStrings[(int)token.type] << ", \""
              << encodeUTF8(token.content) << "\", " << token.line << ")";
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
