#ifndef TANUKIPROJECT_TOKENPROCESSOR_H
#define TANUKIPROJECT_TOKENPROCESSOR_H

#include "Token.h"
#include <vector>

class TokenProcessor {
  public:
    virtual ~TokenProcessor() = default;

    virtual vector<Token> process(const vector<Token> &input) = 0;
};

class IndentationTokenProcessor : public TokenProcessor {
  public:
    vector<Token> process(const vector<Token> &input) override;
};

#endif // TANUKIPROJECT_TOKENPROCESSOR_H
