#include <iostream>

#include "gtest/gtest.h"

#include "Tokenizer.h"
#include "Parser.h"
#include "Evaluate.h"

TEST(tokenizer, simple_test)
{
	auto stringInput = StringInputSource(L"関数、フィボナッチ（番号）");
	auto testTokenizer = FileTokenizer(&stringInput);
	auto allTokens = testTokenizer.getAllTokens();
	EXPECT_EQ(7, allTokens.size());
	EXPECT_EQ(allTokens[0], Token(TokenType::FUNC, L"関数",1));
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
		L"関数、フィボナッチ（番号）\n"
		L"　戻り、１２３"
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto allTokens = testTokenizer.getAllTokens();
	EXPECT_EQ(12, allTokens.size());
	EXPECT_EQ(allTokens[6], Token(TokenType::NEWL, L"",2));
	EXPECT_EQ(allTokens[7], Token(TokenType::INDENT, L"",2));
	EXPECT_EQ(allTokens[8], Token(TokenType::RETURN, L"戻り",2));
	EXPECT_EQ(allTokens[9], Token(TokenType::COMMA, L"、",2));
	EXPECT_EQ(allTokens[10], Token(TokenType::NUMBER, L"１２３",2));
	EXPECT_EQ(allTokens[11], Token(TokenType::END, L"",2));
}

TEST(tokenizer, functions)
{
	auto stringInput = StringInputSource(
		L"あ（い（）、う（））"
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto allTokens = testTokenizer.getAllTokens();
	EXPECT_EQ(11, allTokens.size());
	EXPECT_EQ(allTokens[0], Token(TokenType::SYMBOL, L"あ",1));
	EXPECT_EQ(allTokens[1], Token(TokenType::LPAREN, L"（",1));
	EXPECT_EQ(allTokens[2], Token(TokenType::SYMBOL, L"い",1));
	EXPECT_EQ(allTokens[3], Token(TokenType::LPAREN, L"（",1));
	EXPECT_EQ(allTokens[4], Token(TokenType::RPAREN, L"）",1));
	EXPECT_EQ(allTokens[5], Token(TokenType::COMMA, L"、",1));
	EXPECT_EQ(allTokens[6], Token(TokenType::SYMBOL, L"う",1));
	EXPECT_EQ(allTokens[7], Token(TokenType::LPAREN, L"（",1));
	EXPECT_EQ(allTokens[8], Token(TokenType::RPAREN, L"）",1));
	EXPECT_EQ(allTokens[9], Token(TokenType::RPAREN, L"）",1));
}

TEST(parser, functions)
{
	auto stringInput = StringInputSource(
		L"あ（い（）、４５６、う（１２３））（え）"
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	string treeString = tree->children[0]->toString();
	// cout << treeString << endl;
	string expected = (
		"CALL\n"
		" TERMINAL symbol：”あ”、1列\n"
		" CALL_TAIL\n"
		"  ARGS\n"
		"   CALL\n"
		"    TERMINAL symbol：”い”、1列\n"
		"    CALL_TAIL\n"
		"     ARGS\n"
		"   TERMINAL number：”４５６（456）”、1列\n"
		"   CALL\n"
		"    TERMINAL symbol：”う”、1列\n"
		"    CALL_TAIL\n"
		"     ARGS\n"
		"      TERMINAL number：”１２３（123）”、1列\n"
		"  CALL_TAIL\n"
		"   ARGS\n"
		"    TERMINAL symbol：”え”、1列\n"
	);
	EXPECT_EQ(expected, treeString);
}
