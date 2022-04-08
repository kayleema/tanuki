#include "MatcherComparison.h"
#include "Lexer/LexerConstants.h"
#include <iostream>
#include <unordered_set>

MatcherResult MatcherComparison::match(wchar_t first, int currentLineNumber,
                                       InputSource *input) {
    static const auto comparisonChars =
        std::unordered_set<wchar_t>({greaterThan, lessThan, assign, notSign});

    if (comparisonChars.count(first) &&
        comparisonChars.count(input->peekChar())) {
        wstring comparison(1, first);
        comparison.append(1, input->getChar());
        if (comparison == L"＝＝") {
            return MatcherResult(Token(EQ, L"＝＝", currentLineNumber));
        } else if (comparison == L"＞＝") {
            return MatcherResult(Token(GEQ, L"＞＝", currentLineNumber));
        } else if (comparison == L"＜＝") {
            return MatcherResult(Token(LEQ, L"＜＝", currentLineNumber));
        } else if (comparison == L"！＝") {
            return MatcherResult(Token(NEQ, L"！＝", currentLineNumber));
        } else {
            std::cout << "lexer error : invalid comparison" << std::endl;
        }
    }
    return {};
}