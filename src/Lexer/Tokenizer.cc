#include "Tokenizer.h"
#include <iostream>

vector<Token> Tokenizer::getAllTokens() {
    auto allTokens = vector<Token>();
    do {
        auto newToken = getToken();
        allTokens.push_back(newToken);
    } while (allTokens.back().type != TokenType::END);
    return allTokens;
}

Token InputSourceTokenizer::getToken() {
    wchar_t first = input->getChar();

    for (auto &matcher : matchers) {
        auto match = matcher->match(first, lineNumber, input);
        lineNumber += match.incrementLineNumber;
        if (match.matched) {
            return match.result;
        }
    }

    cout << "ERROR: Tokenizer overrun past end of file. Line " << lineNumber
         << endl;
    return {};
}

vector<Token> InputSourceTokenizer::getAllTokens() {
    auto result = Tokenizer::getAllTokens();
    for (const auto &processor : tokenProcessors){
        result = processor->process(result);
    }
    return result;
}
