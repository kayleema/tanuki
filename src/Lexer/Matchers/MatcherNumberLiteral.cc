#include "MatcherNumberLiteral.h"
#include "Lexer/Matcher.h"
#include "TextInput/UnicodeConversion.h"

bool charIsNumeric(TnkChar c) {
    return (c <= L'９' && c >= L'０') || c == L'。';
}

MatcherResult MatcherNumberLiteral::match(TnkChar first, int currentLineNumber,
                                          InputSource *input) {
    if (charIsNumeric(first)) {
        string resultNumber = tnkCharToString(first);
        while (charIsNumeric(input->peekChar()) && !input->eof()) {
            resultNumber.append(tnkCharToString(input->getChar()));
        }
        TokenType tokenType = (resultNumber.find("。") != std::string::npos)
                                  ? TokenType::NUMBER_FLOAT
                                  : TokenType::NUMBER;

        return MatcherResult(
            Token(tokenType, string(resultNumber), currentLineNumber));
    }
    return {};
}
