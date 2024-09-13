#include "MatcherComment.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"

MatcherResult MatcherComment::match(TnkChar first, int currentLineNumber,
                                    InputSource *input) {
    if (first == sharp) {
        while (input->getChar() != newline) {
            if (input->eof()) {
                return MatcherResult(
                    Token(TokenType::END, "", currentLineNumber));
            }
        }
        return MatcherResult(Token(TokenType::NEWL, "", currentLineNumber + 1),
                             true);
    }
    return {};
}
