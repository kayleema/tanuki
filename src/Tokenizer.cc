#include <utility>

#include "Tokenizer.h"
#include <unordered_map>
#include <unordered_set>

// Needed for GCC
#include <locale>

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
const wchar_t lbrace = L'【';
const wchar_t rbrace = L'】';
const wchar_t comma = L'、';
const wchar_t space = L'　';
const wchar_t assign = L'＝';
const wchar_t dot = L'・';
const wchar_t newline = L'\n';
const wchar_t minuszenkaku = L'－';
const wchar_t star = L'＊';
const wchar_t colon = L'：';
const wchar_t sharp = L'＃';
const wchar_t plusSign = L'＋';
const wchar_t slash = L'／';
const wchar_t nullChar = L'\0';
const wchar_t greaterThan = L'＞';
const wchar_t lessThan = L'＜';
const wchar_t notSign = L'！';
const wchar_t nami = L'〜';

const unordered_set<wchar_t> symbolicChars(
        {
                lparen,
                rparen,
                comma,
                space,
                newline,
                assign,
                dot,
                star,
                colon,
                sharp,
                minuszenkaku,
                plusSign,
                nullChar,
                slash,
                greaterThan,
                notSign,
                lessThan,
                lbrace,
                rbrace,
                nami
        });

const unordered_map<wchar_t, const TokenType> charToTokenTypeMap(
        {
                {lparen,       TokenType::LPAREN},
                {lparen,       TokenType::LPAREN},
                {lbrace,       TokenType::LBRACE},
                {rbrace,       TokenType::RBRACE},
                {rparen,       TokenType::RPAREN},
                {comma,        TokenType::COMMA},
                {assign,       TokenType::ASSIGN},
                {dot,          TokenType::DOT},
                {minuszenkaku, TokenType::MINUS},
                {star,         TokenType::STAR},
                {colon,        TokenType::COLON},
                {plusSign,     TokenType::PLUS},
                {slash,        TokenType::SLASH},
                {greaterThan,  TokenType::GT},
                {lessThan,     TokenType::LT},
                {nami,         TokenType::NAMI},
        });

// Tokenizer Implementation
const unordered_map<wstring, TokenType> identifiers(
        {
                {L"関数",   TokenType::FUNC},
                {L"返す",   TokenType::RETURN},
                {L"もし",   TokenType::IF},
                {L"他でもし", TokenType::ELIF},
                {L"その他",  TokenType::ELSE},
                {L"導入",   TokenType::IMPORT},
                {L"確認", TokenType::ASSERT},
                {L"外側", TokenType::EXTERN}
        });

bool charIsSymbolic(wchar_t c) {
    return symbolicChars.count(c) == 0;
}

bool charIsNumberic(wchar_t c) {
    return c <= L'９' && c >= L'０';
}

long parseNumeric(wstring s) {
    if (s.length() == 0) {
        return 0;
    }
    long result = s.back() - L'０';
    s.pop_back();
    return result + parseNumeric(s) * 10;
}

const char* tokenTypeToString(TokenType type) {
    return TokenTypeStrings[(int) type];
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
    result << u8"：”" << encodeUTF8(content);
    if (type == TokenType::NUMBER) {
        result << u8"（" << number << u8"）";
    }
    result << u8"”、" << line << u8"行目";
    return result.str();
}

bool Token::operator==(const Token &rhs) const {
    return type == rhs.type && content == rhs.content && line == rhs.line;
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    return os << "Token("
              << TokenTypeStrings[(int) token.type]
              << ", \""
              << encodeUTF8(token.content)
              << "\", "
              << token.line
              << ")";
}

bool isComplete(const vector<Token> &tokens) {
    int openParenCount = 0;
    int indentCount = 0;
    bool unstartedFunction = false;
    for (const auto &token : tokens) {
        if (token.type == TokenType::LPAREN) {
            openParenCount++;
        } else if (token.type == TokenType::RPAREN) {
            openParenCount--;
        } else if (token.type == TokenType::FUNC) {
            unstartedFunction = true;
        } else if (token.type == TokenType::INDENT) {
            unstartedFunction = false;
            indentCount++;
        } else if (token.type == TokenType::DEDENT) {
            unstartedFunction = false;
            indentCount--;
        }
    }
    return !unstartedFunction && openParenCount == 0 && indentCount == 0;
}

vector<Token> Tokenizer::getAllTokens() {
    vector<Token> result;
    Token current;
    do {
        current = getToken();
        result.push_back(current);
    } while (current.type != TokenType::END);
    return result;
}

Token InputSourceTokenizer::getToken() {
    if (!nextTokens.empty()) {
        Token result = nextTokens.front();
        nextTokens.pop();
        return result;
    }
    wchar_t first = input->getChar();
    if (input->eof()) {
        return Token(TokenType::END, L"", lineNumber);
    }
    unordered_set<wchar_t> comparisonChars({greaterThan, lessThan, assign, notSign});
    if (comparisonChars.count(first) && comparisonChars.count(input->peekChar())) {
        wstring comparison(1, first);
        comparison.append(1, input->getChar());
        if (comparison == L"＝＝") {
            return Token(TokenType::EQ, L"＝＝", lineNumber);
        } else if (comparison == L"＞＝") {
            return Token(TokenType::GEQ, L"＞＝", lineNumber);
        } else if (comparison == L"＜＝") {
            return Token(TokenType::LEQ, L"＜＝", lineNumber);
        } else if (comparison == L"！＝") {
            return Token(TokenType::NEQ, L"！＝", lineNumber);
        } else {
            cout << "lexer error : invalid comparison" << endl;
        }
    }
    if (charToTokenTypeMap.count(first)) {
        return Token(charToTokenTypeMap.at(first), wstring(1, first),
                     lineNumber);
    }
    if (first == sharp) {
        while (input->getChar() != newline) {
            if (input->eof()) {
                return Token(TokenType::END, L"", lineNumber);
            }
        }
        lineNumber++;
        return Token(TokenType::NEWL, L"", lineNumber);
    }
    if (first == newline) {
        lineNumber++;
        if (input->peekChar() != L'　' && input->peekChar() != L'\n' &&
            indentLevel > 0) {
            // Dedent to zero case
            for (int i = 0; i < indentLevel; i++) {
                nextTokens.push(Token(TokenType::DEDENT, L"", lineNumber));
            }
            indentLevel = 0;
            return Token(TokenType::NEWL, L"", lineNumber);
        }
        return Token(TokenType::NEWL, L"", lineNumber);
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
                cout << u8"エラー：文字列を読みながら、ファイルの終わり（ＥＯＦ）" << endl;
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
