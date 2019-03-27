#include <iostream>

#include "gtest/gtest.h"

#include "Tokenizer.h"

TEST(tokenizer, simple_test)
{
	auto stringInput = StringInputSource(L"変数、フィボナッチ（番号）");
	auto testTokenizer = FileTokenizer(&stringInput);
	auto allTokens = testTokenizer.getAllTokens();
	EXPECT_EQ(7, allTokens.size());
	EXPECT_EQ(allTokens[0], Token(TokenType::SYMBOL, L"変数",1));
	EXPECT_EQ(allTokens[1], Token(TokenType::COMMA, L"、",1));
	EXPECT_EQ(allTokens[2], Token(TokenType::SYMBOL, L"フィボナッチ",1));
	EXPECT_EQ(allTokens[3], Token(TokenType::LPAREN, L"（",1));
	EXPECT_EQ(allTokens[4], Token(TokenType::SYMBOL, L"番号",1));
	EXPECT_EQ(allTokens[5], Token(TokenType::RPAREN, L"）",1));
	EXPECT_EQ(allTokens[6], Token(TokenType::END, L"",1));
}

TEST(tokenizer, multiline)
{
	auto stringInput = StringInputSource(
		L"変数、フィボナッチ（番号）\n"
		L"　戻り、１２３"
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto allTokens = testTokenizer.getAllTokens();
	EXPECT_EQ(12, allTokens.size());
	EXPECT_EQ(allTokens[6], Token(TokenType::NEWL, L"",2));
	EXPECT_EQ(allTokens[7], Token(TokenType::INDENT, L"",2));
	EXPECT_EQ(allTokens[8], Token(TokenType::SYMBOL, L"戻り",2));
	EXPECT_EQ(allTokens[9], Token(TokenType::COMMA, L"、",2));
	EXPECT_EQ(allTokens[10], Token(TokenType::NUMBER, L"１２３",2));
}
