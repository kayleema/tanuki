#include "MatcherEOF.h"
#include "Lexer/Matcher.h"

MatcherResult MatcherEOF::match(wchar_t first, int currentLineNumber,
                                InputSource *input) {
    if (input->eof()) {
        return MatcherResult(Token(TokenType::END, L"", currentLineNumber));
    }
    return {};
}
