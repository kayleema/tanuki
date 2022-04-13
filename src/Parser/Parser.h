#include <utility>

#ifndef PARSER_H
#define PARSER_H

#include "Lexer/Tokenizer.h"
#include "Logger.h"
#include "SyntaxNode.h"

class Parser {
    Tokenizer *lexer;
    vector<Token> allTokens;
    PinponLogger *logger;
    int currentTokenIndex{};
public:
    explicit Parser(Tokenizer *t, PinponLogger *l) : lexer(t), logger(l) {}

    SyntaxNode *run();

    SyntaxNode *run_text();

    SyntaxNode *run_statement();

    SyntaxNode *run_return();

    SyntaxNode *run_import();

    SyntaxNode *run_nonlocal();

    SyntaxNode *run_if();

    SyntaxNode *run_assert();

    SyntaxNode *run_assign();

    SyntaxNode *run_function();

    SyntaxNode *run_expression();

    SyntaxNode *run_infix_expression();

    SyntaxNode *run_infix_comparison_expression();

    SyntaxNode *run_infix_additive_expression();

    SyntaxNode *run_infix_multiplicative_expression();

    SyntaxNode *run_args();

    SyntaxNode *run_expression_tail();

    bool accept(TokenType type, Token *out = nullptr);

    bool accept(const vector<TokenType> &types, const vector<Token *> &outs);

    bool accept(const vector<TokenType> &types, const vector<Token *> &outs,
                const vector<TokenType> &reject);

    bool expect(TokenType type);

    Token currentToken();

private:
    void logInternal(string message);
};

#endif
