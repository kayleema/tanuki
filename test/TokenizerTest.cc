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

TEST(eval, functions)
{
	auto stringInput = StringInputSource(
		L"足す（２、引く（５、２）、４）"
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	SyntaxNode *expr = tree->children[0];
	string treeString = expr->toString();
	// cout << treeString << endl;
	Environment env;
	Value *v = env.eval(expr);
	NumberValue expected(9);
	// cout << ((NumberValue *)v)->value;
	EXPECT_TRUE(expected.equals(v));
}

TEST(eval, user_function)
{
	auto stringInput = StringInputSource(
		L"関数、プラス二（あ）\n"
		L"　戻り、足す（あ、２）\n"
		L""
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	Environment env;
	env.eval(tree);
	Value *functionValue = env.lookup(L"プラス二");
	EXPECT_EQ(ValueType::FUNC, functionValue->type);

	auto stringInput2 = StringInputSource(
		L"プラス二（７）"
	);
	auto testTokenizer2 = FileTokenizer(&stringInput2);
	auto parser2 = Parser(&testTokenizer2);
	SyntaxNode *tree2 = parser2.run();
	SyntaxNode *expr = tree2->children[0];
	Value *v = env.eval(expr);
	NumberValue expected(9);
	EXPECT_TRUE(expected.equals(v));
}

TEST(eval, if_statement)
{
	auto stringInput = StringInputSource(
		L"関数、五番です（番号）\n"
		L"　もし、イコール（番号、５）\n"
		L"　　戻り、１\n"
		L"　戻り、０\n"
		L""
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	Environment env;
	env.eval(tree);
	Value *functionValue = env.lookup(L"五番です");
	EXPECT_EQ(ValueType::FUNC, functionValue->type);

	auto stringInput2 = StringInputSource(
		L"五番です（５）"
	);
	testTokenizer = FileTokenizer(&stringInput2);
	parser = Parser(&testTokenizer);
	tree = parser.run();
	SyntaxNode *expr = tree->children[0];
	Value *v = env.eval(expr);
	NumberValue expected(1);
	EXPECT_TRUE(expected.equals(v));

	auto stringInput3 = StringInputSource(
		L"五番です（４）"
	);
	testTokenizer = FileTokenizer(&stringInput3);
	parser = Parser(&testTokenizer);
	tree = parser.run();
	expr = tree->children[0];
	v = env.eval(expr);
	expected = NumberValue(0);
	EXPECT_TRUE(expected.equals(v));
}
