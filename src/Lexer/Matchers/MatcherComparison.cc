#include "MatcherComparison.h"
#include "Lexer/LexerConstants.h"
#include <iostream>
#include <unordered_set>
#include "TextInput/UnicodeConversion.h"

MatcherResult MatcherComparison::match(TnkChar first, int currentLineNumber,
                                       InputSource *input) {
    static const auto comparisonChars =
        std::unordered_set<TnkChar>({greaterThan, lessThan, assign, notSign});

    if (comparisonChars.count(first) &&
        comparisonChars.count(input->peekChar())) {
        auto second = input->getChar();

        if (first == assign && second == assign) {
            return MatcherResult(Token(EQ, "＝＝", currentLineNumber));
        } else if (first == greaterThan && second == assign) {
            return MatcherResult(Token(GEQ, "＞＝", currentLineNumber));
        } else if (first == lessThan && second == assign) {
            return MatcherResult(Token(LEQ, "＜＝", currentLineNumber));
        } else if (first == notSign && second == assign) {
            return MatcherResult(Token(NEQ, "！＝", currentLineNumber));
        } else {
            std::cout << "lexer error : invalid comparison" << std::endl;
        }
    }
    return {};
}