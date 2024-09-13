#include "MatcherEOF.h"
#include "Lexer/Matcher.h"

MatcherResult MatcherEOF::match(TnkChar, int currentLineNumber,
                                InputSource *input) {
    if (input->eof()) {
        return MatcherResult(Token(TokenType::END, "", currentLineNumber));
    }
    return {};
}
