#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "InputSource.h"
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

long parseNumeric(wstring s);

float parseNumericFloat(wstring s);

bool charIsSymbolic(wchar_t c);

class Tokenizer {
  public:
    virtual vector<Token> getAllTokens();

    virtual Token getToken() = 0;
};

class InputSourceTokenizer : public Tokenizer {
    InputSource *input;
    int lineNumber = 1;

  public:
    explicit InputSourceTokenizer(InputSource *input) : input(input) {}

    virtual Token getToken() override;
};

class BufferedTokenizer : public Tokenizer {
    int position = 0;
    vector<Token> tokens;

  public:
    explicit BufferedTokenizer(vector<Token> tokens) {
        this->tokens = std::move(tokens);
    }

    virtual Token getToken() override { return tokens[position++]; }
};

bool isComplete(const vector<Token> &);

#endif