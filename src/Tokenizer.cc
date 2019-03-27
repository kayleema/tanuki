#include "Tokenizer.h"

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

static const char * TokenTypeStrings[] = {
	"lparen", "rparen", "comma", "symbol", "end", "start", "space", 
	"newl", "indent", "dedent", "number"
};


Token::Token(TokenType type, wstring _content, int line)
	: type(type), content(_content), line(line) {

}

string Token::toString() const {
	ostringstream result("");
	result << TokenTypeStrings[(int)type] << "：”" << encodeUTF8(content) << "”、" 
		<< line << "列";
	return result.str();
}

vector<Token> Tokenizer::getAllTokens() {
	vector<Token> result;
	Token current;
	do {
		current = getToken();
		result.push_back(current);
	} while (current.type != TokenType::END);
	return result;
}

bool Token::operator==(const Token& rhs) const{
	return type == rhs.type && content == rhs.content && line == rhs.line;
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
	return Token(TokenType::SYMBOL, wstring(resultSymbol), linenumber);
}
