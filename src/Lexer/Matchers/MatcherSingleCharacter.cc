#include "MatcherSingleCharacter.h"
#include "Lexer/LexerConstants.h"
#include <unordered_map>
#include "TextInput/UnicodeConversion.h"

MatcherResult MatcherSingleCharacter::match(TnkChar first,
                                            int currentLineNumber,
                                            InputSource *) {
    static const std::unordered_map<TnkChar, const TokenType>
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
                                   tnkCharToString(first),
                                   currentLineNumber));
    }
    return {};
}
