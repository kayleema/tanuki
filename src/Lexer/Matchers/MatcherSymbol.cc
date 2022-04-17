#include "MatcherSymbol.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"
#include "TextInput/UnicodeConversion.h"
#include <unordered_map>
#include <unordered_set>

bool charIsSymbolic(wchar_t c) {
    static const std::unordered_set<wchar_t> symbolicChars(
        {lparen,       rparen,   comma,    space,  newline,
         assign,       dot,      star,     colon,  sharp,
         minuszenkaku, plusSign, nullChar, slash,  greaterThan,
         notSign,      lessThan, lbrace,   rbrace, nami});

    return c != 0 && symbolicChars.count(c) == 0;
}

MatcherResult MatcherSymbol::match(wchar_t first, int currentLineNumber,
                                   InputSource *input) {
    wstring resultSymbol = wstring(1, first);
    while (charIsSymbolic(input->peekChar()) && !input->eof()) {
        resultSymbol.push_back(input->getChar());
    }

    auto resultIdentifier = matchIdentifier(resultSymbol, currentLineNumber);
    if (resultIdentifier.matched) {
        return resultIdentifier;
    }

    auto resultNumber = matchKanjiNumber(resultSymbol, currentLineNumber);
    if (resultNumber.matched) {
        return resultNumber;
    }

    return MatcherResult(
        Token(TokenType::SYMBOL, wstring(resultSymbol), currentLineNumber));
}

MatcherResult MatcherSymbol::matchIdentifier(const wstring &tokenString,
                                             int currentLineNumber) {
    const std::unordered_map<wstring, TokenType> identifiers(
        {{L"関数", TokenType::FUNC},
         {L"返す", TokenType::RETURN},
         {L"もし", TokenType::IF},
         {L"あるいは", TokenType::ELIF},
         {L"その他", TokenType::ELSE},
         {L"導入", TokenType::IMPORT},
         {L"確認", TokenType::ASSERT},
         {L"外側", TokenType::EXTERN}});

    if (identifiers.count(tokenString)) {
        const TokenType newTokenType = identifiers.at(tokenString);
        return MatcherResult(
            Token(newTokenType, wstring(tokenString), currentLineNumber));
    }
    return MatcherResult();
}

MatcherResult MatcherSymbol::matchKanjiNumber(const wstring &tokenString,
                                              int currentLineNumber) {
    static const auto kanjiChars = wstring(L"零一二三四五六七八九十百千万億兆京");
    for (wchar_t const &c : tokenString) {
        if (kanjiChars.find(c) == std::string::npos) {
            return MatcherResult();
        }
    }

    return MatcherResult(
        Token(
        TokenType::NUMBER,
        tokenString,
        currentLineNumber,
        parseKanjiNumber(tokenString)
    ));
}


long MatcherSymbol::parseKanjiNumber(wstring input) {
    static const auto kanjiTen = wstring(L"零一二三四五六七八九十");
    if (input.length() == 1) {
        auto pos = kanjiTen.find(input[0]);
        if (pos != std::string::npos) {
            return long(pos);
        }
    }

    static const auto kanjiPowers = wstring(L"京兆億万千百十");
    long i = 10000000000000000;
    for (const wchar_t &power : kanjiPowers) {
        auto splitPoint = input.find(power);
        if (splitPoint != std::string::npos) {
            auto lhs = input.substr(0, splitPoint);
            auto rhs = input.substr(splitPoint + 1);
            long lhsNum = 1;
            long rhsNum = 0;
            if (lhs.length() > 0) {
                lhsNum = parseKanjiNumber(lhs);
            }
            if (rhs.length() > 0) {
                rhsNum = parseKanjiNumber(rhs);
            }
            return lhsNum * i + rhsNum;
        }
        if (i > 10000) {
            i /= 10000;
        } else {
            i /= 10;
        }
    }

    return 0;
}
