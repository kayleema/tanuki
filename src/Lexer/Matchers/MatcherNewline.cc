#include "MatcherNewline.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"

MatcherResult MatcherNewline::match(TnkChar first, int currentLineNumber,
                                    InputSource *) {
    if (first == newline) {
        return MatcherResult(Token(TokenType::NEWL, "", currentLineNumber + 1),
                             true);
    }
    return {};
}
