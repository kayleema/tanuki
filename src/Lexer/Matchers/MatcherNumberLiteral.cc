#include "MatcherNumberLiteral.h"
#include "Lexer/Matcher.h"

bool charIsNumeric(wchar_t c) {
    return (c <= L'９' && c >= L'０') || c == L'。';
}

MatcherResult MatcherNumberLiteral::match(wchar_t first, int currentLineNumber,
                                          InputSource *input) {
    if (charIsNumeric(first)) {
        wstring resultNumber = wstring(1, first);
        while (charIsNumeric(input->peekChar()) && !input->eof()) {
            resultNumber.push_back(input->getChar());
        }
        TokenType tokenType = (resultNumber.find(L'。') != std::string::npos)
                                  ? TokenType::NUMBER_FLOAT
                                  : TokenType::NUMBER;

        return MatcherResult(
            Token(tokenType, wstring(resultNumber), currentLineNumber));
    }
    return {};
}
