#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <utility>
#include <vector>

using namespace std;

enum TokenType {
    LPAREN,
    RPAREN,
    COMMA,
    SYMBOL,
    END,
    START,
    SPACE,
    NEWL,
    INDENT,
    DEDENT,
    NUMBER,
    NUMBER_FLOAT,
    FUNC,
    RETURN,
    IF,
    ELSE,
    STRING,
    ASSIGN,
    DOT,
    ELIF,
    MINUS,
    IMPORT,
    STAR,
    COLON,
    PLUS,
    SLASH,
    EQ,
    LEQ,
    GEQ,
    LT,
    GT,
    NEQ,
    ASSERT,
    LBRACE,
    RBRACE,
    EXTERN,
    NAMI
};

static const char *const TokenTypeStrings[] = {
    "lparen",   "rparen", "comma",  "symbol", "end",    "start",
    "space",    "newl",   "indent", "dedent", "number", "number_float",
    "function", "return", "if",     "else",   "string", "assign",
    "dot",      "elif",   "minus",  "import", "star",   "colon",
    "plus",     "slash",  "eq",     "leq",    "geq",    "lt",
    "gt",       "neq",    "assert", "lbrace", "rbrace", "extern",
    "nami"};
class Token {
  public:
    Token() : type(TokenType::START), content(L""), line(0) {}

    Token(TokenType type, wstring _content, int line);

    Token(TokenType type, wstring _content, int line, long number)
        : type(type), content(std::move(_content)), line(line), number(number) {
    }

    TokenType type;
    wstring content;
    int line;
    long number{};
    double numberFloat{};

    string toString() const;

    bool operator==(const Token &rhs) const;

    bool operator!=(const Token &rhs) const { return !(*this == rhs); };

    friend ostream &operator<<(ostream &os, const Token &token);
};

const char *tokenTypeToString(TokenType type);

string tokenListToTypeList(std::vector<Token> list);

#endif // TOKEN_H
