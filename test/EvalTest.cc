#include "gtest/gtest.h"

#include "Tokenizer.h"
#include "Parser.h"
#include "Evaluate.h"

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
	Context context;
	Environment *env = new Environment(&context);
	Value *v = env->eval(expr);
	NumberValue expected(9);
	// cout << ((NumberValue *)v)->value;
	EXPECT_TRUE(expected.equals(v));

	context.cleanup();
}

TEST(eval, user_function)
{
	auto stringInput = StringInputSource(
		L"関数、プラス二（あ）\n"
		L"　返す、足す（あ、２）\n"
		L""
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	Context context;
	Environment env(&context);
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
		L"　　返す、１\n"
		L"　返す、０\n"
		L""
	);
	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	Context context;
	Environment env(&context);
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

TEST(program, fibonacci) {
	auto stringInput = StringInputSource(
		L"関数、フィボナッチ（号）\n"
		L"　もし、イコール（号、１）\n"
		L"　　返す、１\n"
		L"　もし、イコール（号、０）\n"
		L"　　返す、１\n"
		L"　返す、足す（フィボナッチ（引く（号、１））、フィボナッチ（引く（号、２）））\n"
		L"\n"
		L"フィボナッチ（７）\n"
		L"フィボナッチ（１６）\n"
	);


	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	Context context;
	Environment env(&context);
	env.eval(tree->children[0]);

	Value *v = env.eval(tree->children[1]);
	Value *expected = new NumberValue(21);
	EXPECT_TRUE(expected->equals(v));

	v = env.eval(tree->children[2]);
	expected = new NumberValue(1597);
	EXPECT_TRUE(expected->equals(v));
}

TEST(program, string_eq) {
	auto stringInput = StringInputSource(
		L"あ＝イコール（「ほげ」、「ほけ」）\n"
		L"い＝イコール（「ほげ」、「ほげ」）\n"
	);


	auto testTokenizer = FileTokenizer(&stringInput);
	auto parser = Parser(&testTokenizer);
	SyntaxNode *tree = parser.run();
	Context context;
	Environment env(&context);
	env.eval(tree);

	EXPECT_FALSE(env.lookup(L"あ")->isTruthy());
	EXPECT_TRUE(env.lookup(L"い")->isTruthy());

	// v = env.eval(tree->children[2]);
	// expected = new NumberValue(1597);
	// EXPECT_TRUE(expected->equals(v));
}
