#ifndef TANUKIPROJECT_GENERICPARSER_H
#define TANUKIPROJECT_GENERICPARSER_H
#include "Logger.h"
#include "SyntaxNode.h"

class GenericParser {
    vector<Token> allTokens;
    PinponLogger *logger;
    int currentTokenIndex{};

  public:
    explicit GenericParser(PinponLogger *l) : logger(l) {}

    // Returns full syntax tree for tokens
    SyntaxNode *run(vector<Token> inputTokens);

    // Return true if all tokens were consumed while parsing.
    bool isComplete() { return currentToken().type == TokenType::END; }

  protected:
    // To be overridden in subclasses to parse subtrees
    virtual SyntaxNode *run_text() = 0;

    // In the case that the next token type matches "type": function returns
    // true, consumes token, writes the token to *out if out is non-null.
    // Otherwise, returns false and does not consume any tokens
    bool accept(TokenType type, Token *out = nullptr);

    // Same as above, but can match and return the next N tokens.
    bool accept(const vector<TokenType> &types, const vector<Token *> &outs);

    // Same as above, but adds the condition that after consuming N "types" the
    // following M "reject" types must not match. (where N is length of types
    // and M is length of rejects)
    bool accept(const vector<TokenType> &types, const vector<Token *> &outs,
                const vector<TokenType> &reject);

    // Consume one token requiring that it match "type". In the case that it
    // does not match, function returns false and logs a parsing error.
    bool expect(TokenType type);

    // Current token next up to be consumed.
    Token currentToken();

    // Utility function for logging parser messages.
    void logInternal(string message);
};

#endif // TANUKIPROJECT_GENERICPARSER_H
