#include "MatcherSingleCharacter.h"
#include "Lexer/LexerConstants.h"
#include <unordered_map>

MatcherResult MatcherSingleCharacter::match(wchar_t first,
                                            int currentLineNumber,
                                            InputSource *input) {
    static const std::unordered_map<wchar_t, const TokenType>
        charToTokenTypeMap({{lparen, TokenType::LPAREN},
                            {lparen, TokenType::LPAREN},
                            {lbrace, TokenType::LBRACE},
                            {rbrace, TokenType::RBRACE},
                            {rparen, TokenType::RPAREN},
                            {comma, TokenType::COMMA},
                            {assign, TokenType::ASSIGN},
                            {dot, TokenType::DOT},
                            {minuszenkaku, TokenType::MINUS},
                            {star, TokenType::STAR},
                            {colon, TokenType::COLON},
                            {plusSign, TokenType::PLUS},
                            {slash, TokenType::SLASH},
                            {greaterThan, TokenType::GT},
                            {lessThan, TokenType::LT},
                            {nami, TokenType::NAMI},
                            {L'　', TokenType::INDENT}});
    if (charToTokenTypeMap.count(first)) {
        return MatcherResult(Token(charToTokenTypeMap.at(first),
                                   wstring(1, first), currentLineNumber));
    }
    return {};
}
