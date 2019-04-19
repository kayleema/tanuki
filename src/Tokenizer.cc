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
const wchar_t lsquare = L'「';
const wchar_t rsquare = L'」';
const wchar_t comma = L'、';
const wchar_t space = L'　';
const wchar_t assign = L'＝';
const wchar_t dot = L'・';
const wchar_t newline = L'\n';
const wchar_t minuszenkaku = L'－';

bool charIsSymbolic(wchar_t c) {
	return 
		c != lparen &&
		c != rparen &&
		c != comma &&
		c != space &&
		c != newline &&
		c != assign &&
		c != dot;
}

bool charIsNumberic(wchar_t c) {
	return c <= L'９' && c >= L'０';
}

long parseNumeric(wstring s) {
	// cout << encodeUTF8(s) << result << endl;
	if (s.length() == 0) {
		return 0;
	}
	long result = s.back() - L'０';
	s.pop_back();
	return result + parseNumeric(s) * 10;
}

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
	{L"関数", TokenType::FUNC},
	{L"返す", TokenType::RETURN},
	{L"もし", TokenType::IF},
	{L"他でもし", TokenType::ELIF},
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
	if (!nextTokens.empty()) {
		Token result = nextTokens.front();
		nextTokens.pop();
		return result;
	}
	// cout << "getting" << endl;
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
	if (first == assign) {
		return Token(TokenType::ASSIGN, L"＝", linenumber);
	}
	if (first == dot) {
		return Token(TokenType::DOT, L"・", linenumber);
	}
	if (first == minuszenkaku) {
		return Token(TokenType::MINUS, L"－", linenumber);
	}
	if (first == space) {
		int newIndentLevel = 1;
		while(input->peekChar() == L'　') {
			newIndentLevel++;
			input->getChar();
		}
		if (newIndentLevel == indentLevel) {
			return getToken();
		}
		auto newTokenType = (newIndentLevel > indentLevel) ? 
			TokenType::INDENT : TokenType::DEDENT;
		int difference = abs(newIndentLevel - indentLevel);
		indentLevel = newIndentLevel;
		for (int i = 0; i < difference - 1; i++) {
			nextTokens.push(Token(newTokenType, L"", linenumber));
		}
		return Token(newTokenType, L"", linenumber);
	}
	if (first == newline) {
		linenumber++;
		if (input->peekChar() != L'　' && 
				input->peekChar() != L'\n' && 
				indentLevel > 0) {
			// Dedent to zero case
			for (int i = 0; i < (indentLevel - 1); i++) {
				nextTokens.push(Token(TokenType::DEDENT, L"", linenumber));
			}
			indentLevel = 0;
			nextTokens.push(Token(TokenType::NEWL, L"", linenumber));
			return Token(TokenType::DEDENT, L"", linenumber);
		}
		return Token(TokenType::NEWL, L"", linenumber);
	}
	if (charIsNumberic(first)) {
		wstring resultNumber = wstring(1, first);
		while(charIsNumberic(input->peekChar()) && !input->eof()) {
			resultNumber.push_back(input->getChar());
		}
		return Token(TokenType::NUMBER, wstring(resultNumber), linenumber);
	}
	if (first == lsquare) {
		wstring resultString = wstring(L"");
		wchar_t nextChar;
		while ((nextChar = input->getChar()) != rsquare) {
			resultString.push_back(nextChar);
			if (nextChar == newline) {
				linenumber++;
			}
			if (input->eof()) {
				cout << "エラー：文字列を読みながら、ファイルの終わり（ＥＯＦ）" << endl;
				return Token(TokenType::END, L"", linenumber);
			}
		}
		return Token(TokenType::STRING, wstring(resultString), linenumber);
	}
	wstring resultSymbol = wstring(1, first);
	while (charIsSymbolic(input->peekChar()) && !input->eof()) {
		resultSymbol.push_back(input->getChar());
	}
	if (identifiers.count(resultSymbol)) {
		const TokenType newTokenType = identifiers.at(resultSymbol);
		return Token(newTokenType, wstring(resultSymbol), linenumber);
	}
	return Token(TokenType::SYMBOL, wstring(resultSymbol), linenumber);
}
