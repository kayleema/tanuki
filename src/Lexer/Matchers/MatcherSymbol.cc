#include "MatcherSymbol.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"
#include <unordered_map>
#include <unordered_set>

const std::unordered_set<wchar_t> symbolicChars(
    {lparen,      rparen,  comma,    space,        newline,  assign,   dot,
     star,        colon,   sharp,    minuszenkaku, plusSign, nullChar, slash,
     greaterThan, notSign, lessThan, lbrace,       rbrace,   nami});

bool charIsSymbolic(wchar_t c) { return symbolicChars.count(c) == 0; }

const std::unordered_map<wstring, TokenType>
    identifiers({{L"関数", TokenType::FUNC},
                 {L"返す", TokenType::RETURN},
                 {L"もし", TokenType::IF},
                 {L"あるいは", TokenType::ELIF},
                 {L"その他", TokenType::ELSE},
                 {L"導入", TokenType::IMPORT},
                 {L"確認", TokenType::ASSERT},
                 {L"外側", TokenType::EXTERN}});

MatcherResult MatcherSymbol::match(wchar_t first, int currentLineNumber,
                                   InputSource *input) {
    wstring resultSymbol = wstring(1, first);
    while (charIsSymbolic(input->peekChar()) && !input->eof()) {
        resultSymbol.push_back(input->getChar());
    }
    if (identifiers.count(resultSymbol)) {
        const TokenType newTokenType = identifiers.at(resultSymbol);
        return MatcherResult(
            Token(newTokenType, wstring(resultSymbol), currentLineNumber));
    }
    return MatcherResult(
        Token(TokenType::SYMBOL, wstring(resultSymbol), currentLineNumber));
}
