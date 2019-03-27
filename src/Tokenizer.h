#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <iostream>
#include <vector>
#include <codecvt>
#include <clocale>
#include <cwchar>
#include <sstream>
#include "InputSource.h"

using namespace std;

string encodeUTF8(const wstring &in);
bool charIsSymbolic(wchar_t c);

enum TokenType {
	LPAREN, RPAREN, COMMA, SYMBOL, END, START, SPACE, NEWL, INDENT, DEDENT, 
	NUMBER, FUNC, RETURN, IF, ELSE
};

class Token {
public:
	Token() : type(TokenType::START), content(L""), line(0) {}

	Token(TokenType type, wstring _content, int line);

	TokenType type;
	wstring content;
	int line;
	int number;

	string toString() const;
	bool operator==(const Token& rhs) const;
};

class Tokenizer {
public:
	virtual vector<Token> getAllTokens();
	virtual Token getToken() = 0;
};

class FileTokenizer : public Tokenizer {
	InputSource *input;
	int linenumber = 1;
	int indentLevel = 0;
public:
	FileTokenizer(InputSource *input) : input(input) {
	}
	virtual Token getToken();
};

#endif