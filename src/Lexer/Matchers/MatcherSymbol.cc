#include "MatcherSymbol.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"
#include "TextInput/UnicodeConversion.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

bool charIsSymbolic(TnkChar c) {
    static const std::unordered_set<TnkChar> symbolicChars(
        {lparen,       rparen,   comma,    space,  newline,
         assign,       dot,      star,     colon,  sharp,
         minuszenkaku, plusSign, nullChar, slash,  greaterThan,
         notSign,      lessThan, lbrace,   rbrace, nami});

    return c != 0 && symbolicChars.count(c) == 0;
}

MatcherResult MatcherSymbol::match(TnkChar first, int currentLineNumber,
                                   InputSource *input) {
    string resultSymbol = tnkCharToString(first);
    while (charIsSymbolic(input->peekChar()) && !input->eof()) {
        resultSymbol.append(tnkCharToString(input->getChar()));
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
        Token(TokenType::SYMBOL, resultSymbol, currentLineNumber));
}

MatcherResult MatcherSymbol::matchIdentifier(const string &tokenString,
                                             int currentLineNumber) {
    const std::unordered_map<string, TokenType> identifiers(
        {{"関数", TokenType::FUNC},
         {"返す", TokenType::RETURN},
         {"もし", TokenType::IF},
         {"あるいは", TokenType::ELIF},
         {"その他", TokenType::ELSE},
         {"導入", TokenType::IMPORT},
         {"確認", TokenType::ASSERT},
         {"外側", TokenType::EXTERN}});

    if (identifiers.count(tokenString)) {
        const TokenType newTokenType = identifiers.at(tokenString);
        return MatcherResult(
            Token(newTokenType, tokenString, currentLineNumber));
    }
    return {};
}

static const TnkChar kanjiCharsTnk[] = {
    L'零', L'一', L'二', L'三', L'四', L'五', L'六', L'七', L'八',
    L'九', L'十', L'百', L'千', L'万', L'億', L'兆', L'京',
};

MatcherResult MatcherSymbol::matchKanjiNumber(const string &tokenString,
                                              int currentLineNumber) {
    const char *tokenCString = tokenString.c_str();
    TnkChar currentChar = getFirstUtfChar(tokenCString);
    while (currentChar != 0) {
        if (std::find(std::begin(kanjiCharsTnk), std::end(kanjiCharsTnk),
                      currentChar) == std::end(kanjiCharsTnk)) {
            return {};
        }
        tokenCString += getUtfCharSize(*tokenCString);
        currentChar = getFirstUtfChar(tokenCString);
    }

    return MatcherResult(Token(TokenType::NUMBER, tokenString,
                               currentLineNumber,
                               parseKanjiNumber(tokenString)));
}

static const TnkChar kanjiTen[] = {
    L'零',L'一',L'二',L'三',L'四',L'五',L'六',L'七',L'八',L'九',L'十',
};
static const char * const kanjiPowers[] = {
    "京", "兆", "億", "万", "千", "百", "十",
};

long MatcherSymbol::parseKanjiNumber(string input_s) {
    bool isOneChar = input_s.length() == (size_t)getUtfCharSize(input_s[0]);
    if (isOneChar) {
        auto oneChar = getFirstUtfChar(input_s.c_str());
        auto foundPointer =
            find(begin(kanjiTen), end(kanjiTen), oneChar);
        if (foundPointer != end(kanjiTen)) {
            return foundPointer - kanjiTen;
        }
    }

    static const int charSize = getUtfCharSize(kanjiPowers[0][0]);
    long i = 10000000000000000;
    for (const auto &power : kanjiPowers) {
        auto splitPoint = input_s.find(power);
        if (splitPoint != std::string::npos) {
            auto lhs = input_s.substr(0, splitPoint);
            auto rhs = input_s.substr(splitPoint + charSize);
            long lhsNum = 1;
            long rhsNum = 0;
            if (!lhs.empty()) {
                lhsNum = parseKanjiNumber(lhs);
            }
            if (!rhs.empty()) {
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
