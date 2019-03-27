#include "Tokenizer.h"
#include <unordered_map>

string encodeUTF8(const wstring &in) {
	try
    {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
	    return conv1.to_bytes(in);
    }
    catch (const std::range_error & exception)
    {
        return string("XXX");
    }
}

const wchar_t lparen = L'（';
const wchar_t rparen = L'）';
const wchar_t comma = L'、';
const wchar_t space = L'　';
const wchar_t newline = L'\n';

bool charIsSymbolic(wchar_t c) {
	return 
		c != lparen &&
		c != rparen &&
		c != comma &&
		c != space &&
		c != newline;
}

bool charIsNumberic(wchar_t c) {
	return c <= L'９' && c >= L'０';
}

int parseNumeric(wstring s) {
	if (s.length() == 0) {
		return 0;
	}
	int result = s.back() - L'０';
	s.pop_back();
	return result + parseNumeric(s) * 10;
}

static const char * TokenTypeStrings[] = {
	"lparen", "rparen", "comma", "symbol", "end", "start", "space", 
	"newl", "indent", "dedent", "number", "function", "return", "if", "else"
};

Token::Token(TokenType type, wstring _content, int line)
	: type(type), content(_content), line(line) {
	if (type == TokenType::NUMBER) {
		number = parseNumeric(content);
	}
}

string Token::toString() const {
	ostringstream result("");
	result << TokenTypeStrings[(int)type];
	result << "：”" << encodeUTF8(content);
	if (type == TokenType::NUMBER) {
		result << "（" << number << "）";
	}
	result << "”、" << line << "列";
	return result.str();
}

bool Token::operator==(const Token& rhs) const{
	return type == rhs.type && content == rhs.content && line == rhs.line;
}

// Tokenizer Implementation
static const unordered_map<wstring, TokenType> identifiers({
	{L"変数", TokenType::FUNC},
	{L"戻り", TokenType::RETURN},
	{L"もし", TokenType::IF},
	{L"その他", TokenType::ELSE},
});

vector<Token> Tokenizer::getAllTokens() {
	vector<Token> result;
	Token current;
	do {
		current = getToken();
		result.push_back(current);
	} while (current.type != TokenType::END);
	return result;
}

Token FileTokenizer::getToken() {
	wchar_t first = input->getChar();
	if (input->eof()) {
		return Token(TokenType::END, L"", linenumber);
	}
	if (first == lparen) {
		return Token(TokenType::LPAREN, L"（", linenumber);
	}
	if (first == rparen) {
		return Token(TokenType::RPAREN, L"）", linenumber);
	}
	if (first == comma) {
		return Token(TokenType::COMMA, L"、", linenumber);
	}
	if (first == space) {
		int newIndentLevel = 1;
		while(input->peekChar() == L'　') {
			newIndentLevel++;
			input->getChar();
		}
		auto newTokenType = (newIndentLevel > indentLevel) ? 
			TokenType::INDENT : TokenType::DEDENT;
		indentLevel = newIndentLevel;
		return Token(newTokenType, L"", linenumber);
	}
	if (first == newline) {
		linenumber++;
		return Token(TokenType::NEWL, L"", linenumber);
	}
	if (charIsNumberic(first)) {
		wstring resultNumber = wstring(1, first);
		while(charIsNumberic(input->peekChar()) && !input->eof()) {
			resultNumber.push_back(input->getChar());
		}
		return Token(TokenType::NUMBER, wstring(resultNumber), linenumber);
	}
	wstring resultSymbol = wstring(1, first);
	while(charIsSymbolic(input->peekChar()) && !input->eof()) {
		resultSymbol.push_back(input->getChar());
	}
	if (identifiers.count(resultSymbol)) {
		const TokenType newTokenType = identifiers.at(resultSymbol);
		return Token(newTokenType, wstring(resultSymbol), linenumber);
	}
	return Token(TokenType::SYMBOL, wstring(resultSymbol), linenumber);
}
