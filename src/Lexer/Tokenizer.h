#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "TextInput/InputSource.h"
#include "Matcher.h"
#include "Token.h"
#include <clocale>
#include <codecvt>
#include <cwchar>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

string encodeUTF8(const wstring &in);

wstring decodeUTF8(const string &in);

bool charIsSymbolic(wchar_t c);

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