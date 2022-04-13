#include "GenericParser.h"
#include <iostream>

SyntaxNode *GenericParser::run(vector<Token> inputTokens) {
    allTokens = std::move(inputTokens);
    currentTokenIndex = 0;
    return run_text();
}

bool GenericParser::accept(TokenType type, Token *out) {
    if (currentToken().type == type) {
        if (out) {
            *out = currentToken();
        }
        currentTokenIndex++;
        return true;
    }
    return false;
}

bool GenericParser::accept(const vector<TokenType> &types,
                           const vector<Token *> &outs) {
    size_t i = 0;
    for (auto type : types) {
        size_t tokenIndex = (size_t)currentTokenIndex + i;
        if (tokenIndex >= allTokens.size()) {
            break;
        }
        if (allTokens[tokenIndex].type != type) {
            return false;
        }
        i++;
    }
    i = 0;
    for (auto out : outs) {
        if (out) {
            *out = allTokens[currentTokenIndex + i];
        }
        i++;
    }

    currentTokenIndex += types.size();
    return true;
}

bool GenericParser::accept(const vector<TokenType> &types,
                           const vector<Token *> &outs,
                           const vector<TokenType> &rejectTypes) {
    int i = 0;
    for (auto rejectType : rejectTypes) {
        size_t tokenIndex = currentTokenIndex + types.size() + i;
        if (tokenIndex >= allTokens.size()) {
            break;
        }
        if (allTokens[tokenIndex].type == rejectType) {
            return false;
        }
        i++;
    }
    return GenericParser::accept(types, outs);
}

bool GenericParser::expect(TokenType type) {
    if (accept(type)) {
        return true;
    }
    logInternal("意外なトークン：" + currentToken().toString() + "\n");
    logInternal("  希望のトークン型は「" + string(tokenTypeToString(type)) +
                "」\n");
    return false;
}

Token GenericParser::currentToken() { return allTokens[currentTokenIndex]; }

void GenericParser::logInternal(string message) {
    if (logger) {
        logger->log(message);
    } else {
        std::cout << message;
    }
}
