#include "MatcherNewline.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"

MatcherResult MatcherNewline::match(wchar_t first, int currentLineNumber,
                                    InputSource *) {
    if (first == newline) {
        return MatcherResult(Token(TokenType::NEWL, L"", currentLineNumber + 1),
                             true);
    }
    return {};
}
