#include <utility>

#include "Tokenizer.h"
#include <unordered_map>

string encodeUTF8(const wstring &in) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
        return conv1.to_bytes(in);
    }
    catch (const std::range_error &exception) {
        cout << "could not convert to utf8 string " << in.size() << endl;
        return string("XXX");
    }
}

wstring decodeUTF8(const string &in) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
        return conv1.from_bytes(in);
    }
    catch (const std::range_error &exception) {
        return wstring(L"XXX");
    }
}

const wchar_t lparen = L'（';
const wchar_t rparen = L'）';
const wchar_t lsquare = L'「';
const wchar_t rsquare = L'」';
const wchar_t comma = L'、';
const wchar_t space = L'　';
const wchar_t assign = L'＝';
const wchar_t dot = L'・';
const wchar_t newline = L'\n';
const wchar_t minuszenkaku = L'－';
const wchar_t star = L'＊';
const wchar_t colon = L'：';
const wchar_t sharp = L'＃';

bool charIsSymbolic(wchar_t c) {
    return
            c != lparen &&
            c != rparen &&
            c != comma &&
            c != space &&
            c != newline &&
            c != assign &&
            c != dot &&
            c != star &&
            c != colon &&
            c != sharp;
}

bool charIsNumberic(wchar_t c) {
    return c <= L'９' && c >= L'０';
}

long parseNumeric(wstring s) {
    // cout << encodeUTF8(s) << result << endl;
    if (s.length() == 0) {
        return 0;
    }
    long result = s.back() - L'０';
    s.pop_back();
    return result + parseNumeric(s) * 10;
}

Token::Token(TokenType type, wstring _content, int line)
        : type(type), content(std::move(_content)), line(line) {
    if (type == TokenType::NUMBER) {
        number = parseNumeric(content);
    }
}

string Token::toString() const {
    ostringstream result("");
    result << TokenTypeStrings[(int) type];
    result << "：”" << encodeUTF8(content);
    if (type == TokenType::NUMBER) {
        result << "（" << number << "）";
    }
    result << "”、" << line << "列";
    return result.str();
}

bool Token::operator==(const Token &rhs) const {
    return type == rhs.type && content == rhs.content && line == rhs.line;
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    return os << token.toString();
}

// Tokenizer Implementation
const unordered_map<wstring, TokenType> identifiers(
        {
                {L"関数",   TokenType::FUNC},
                {L"返す",   TokenType::RETURN},
                {L"もし",   TokenType::IF},
                {L"他でもし", TokenType::ELIF},
                {L"その他",  TokenType::ELSE},
                {L"導入",   TokenType::IMPORT}
        });

vector<Token> Tokenizer::getAllTokens() {
    vector<Token> result;
    Token current;
    do {
        current = getToken();
        result.push_back(current);
    } while (current.type != TokenType::END);
    return result;
}

Token FileTokenizer::getToken() {
    if (!nextTokens.empty()) {
        Token result = nextTokens.front();
        nextTokens.pop();
        return result;
    }
    wchar_t first = input->getChar();
    if (first == sharp) {
        while (input->getChar() != newline) {
            if (input->eof()) {
                return Token(TokenType::END, L"", lineNumber);
            }
        }
        lineNumber++;
        return Token(TokenType::NEWL, L"", lineNumber);
    }
    if (input->eof()) {
        return Token(TokenType::END, L"", lineNumber);
    }
    if (first == lparen) {
        return Token(TokenType::LPAREN, L"（", lineNumber);
    }
    if (first == rparen) {
        return Token(TokenType::RPAREN, L"）", lineNumber);
    }
    if (first == comma) {
        return Token(TokenType::COMMA, L"、", lineNumber);
    }
    if (first == assign) {
        return Token(TokenType::ASSIGN, L"＝", lineNumber);
    }
    if (first == dot) {
        return Token(TokenType::DOT, L"・", lineNumber);
    }
    if (first == minuszenkaku) {
        return Token(TokenType::MINUS, L"－", lineNumber);
    }
    if (first == star) {
        return Token(TokenType::STAR, L"＊", lineNumber);
    }
    if (first == colon) {
        return Token(TokenType::COLON, L"：", lineNumber);
    }
    if (first == space) {
        int newIndentLevel = 1;
        while (input->peekChar() == L'　') {
            newIndentLevel++;
            input->getChar();
        }
        if (newIndentLevel == indentLevel) {
            return getToken();
        }
        auto newTokenType = (newIndentLevel > indentLevel) ?
                            TokenType::INDENT : TokenType::DEDENT;
        int difference = abs(newIndentLevel - indentLevel);
        indentLevel = newIndentLevel;
        for (int i = 0; i < difference - 1; i++) {
            nextTokens.push(Token(newTokenType, L"", lineNumber));
        }
        return Token(newTokenType, L"", lineNumber);
    }
    if (first == newline) {
        lineNumber++;
        if (input->peekChar() != L'　' &&
            input->peekChar() != L'\n' &&
            indentLevel > 0) {
            // Dedent to zero case
            for (int i = 0; i < (indentLevel - 1); i++) {
                nextTokens.push(Token(TokenType::DEDENT, L"", lineNumber));
            }
            indentLevel = 0;
            nextTokens.push(Token(TokenType::NEWL, L"", lineNumber));
            return Token(TokenType::DEDENT, L"", lineNumber);
        }
        return Token(TokenType::NEWL, L"", lineNumber);
    }
    if (charIsNumberic(first)) {
        wstring resultNumber = wstring(1, first);
        while (charIsNumberic(input->peekChar()) && !input->eof()) {
            resultNumber.push_back(input->getChar());
        }
        return Token(TokenType::NUMBER, wstring(resultNumber), lineNumber);
    }
    if (first == lsquare) {
        wstring resultString = wstring(L"");
        wchar_t nextChar;
        while ((nextChar = input->getChar()) != rsquare) {
            resultString.push_back(nextChar);
            if (nextChar == newline) {
                lineNumber++;
            }
            if (input->eof()) {
                cout << "エラー：文字列を読みながら、ファイルの終わり（ＥＯＦ）" << endl;
                return Token(TokenType::END, L"", lineNumber);
            }
        }
        return Token(TokenType::STRING, wstring(resultString), lineNumber);
    }
    wstring resultSymbol = wstring(1, first);
    while (charIsSymbolic(input->peekChar()) && !input->eof()) {
        resultSymbol.push_back(input->getChar());
    }
    if (identifiers.count(resultSymbol)) {
        const TokenType newTokenType = identifiers.at(resultSymbol);
        return Token(newTokenType, wstring(resultSymbol), lineNumber);
    }
    return Token(TokenType::SYMBOL, wstring(resultSymbol), lineNumber);
}
