#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "TextInput/InputSource.h"
#include "Matcher.h"
#include "Token.h"
#include <vector>

using namespace std;

class Tokenizer {
  public:
    virtual vector<Token> getAllTokens();

    virtual Token getToken() = 0;
};

class InputSourceTokenizer : public Tokenizer {
    InputSource *input;
    int lineNumber = 1;

  protected:
    vector<Matcher *> matchers;

  public:
    explicit InputSourceTokenizer(InputSource *input) : input(input) {}

    Token getToken() override;
};

class TanukiTokenizer : public InputSourceTokenizer {
  public:
    explicit TanukiTokenizer(InputSource *input);

    ~TanukiTokenizer();
};

#endif