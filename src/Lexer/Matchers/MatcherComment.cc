#include "MatcherComment.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"

MatcherResult MatcherComment::match(wchar_t first, int currentLineNumber,
                                    InputSource *input) {
    if (first == sharp) {
        while (input->getChar() != newline) {
            if (input->eof()) {
                return MatcherResult(
                    Token(TokenType::END, L"", currentLineNumber));
            }
        }
        return MatcherResult(Token(TokenType::NEWL, L"", currentLineNumber + 1),
                             true);
    }
    return {};
}
