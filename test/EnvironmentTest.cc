#include "gtest/gtest.h"
#include "fakeit.hpp"
#include "Tokenizer.h"
#include "Parser.h"
#include "Environment.h"
#include "Context.h"

using namespace fakeit;

TEST(eval, functions) {
    auto stringInput = StringInputSource(
            L"足す（２、引く（５、２）、４）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    SyntaxNode *expr = tree->children[0];
    string treeString = expr->toString();

    Context context;
    auto *env = new Environment(&context);
    Value *v = env->eval(expr);
    NumberValue expected(9);

    EXPECT_TRUE(expected.equals(v));

    context.cleanup();
}

TEST(eval, import_statement) {
    Mock<Filesystem> mockFilesystem;
    When(Method(mockFilesystem, getInputSourceForFilename)).Do(
            [](const string &) {
                return unique_ptr<InputSource>(new StringInputSource(
                        L"あ＝７\n"
                ));
            }
    );

    auto stringInput = StringInputSource(
            L"導入、フォルダー・フォルダー２・ファイル名"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    SyntaxNode *expr = tree->children[0];
    string treeString = expr->toString();

    Context context;
    auto *env = new Environment(&context, &mockFilesystem.get());
    env->bind(L"FILE", context.newStringValue(L""));
    env->eval(expr);

    auto value = env
            ->lookup(L"ファイル名")->toDictionaryValue()
            ->get(L"あ");
    EXPECT_EQ(ValueType::NUM, value->type);
    EXPECT_EQ(7, value->toNumberValue()->value);

    context.cleanup();
}

TEST(eval, user_function) {
    auto stringInput = StringInputSource(
            L"関数、プラス二（あ）\n"
            L"　返す、足す（あ、２）\n"
            L""
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
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
    auto testTokenizer2 = InputSourceTokenizer(&stringInput2);
    auto parser2 = Parser(&testTokenizer2);
    SyntaxNode *tree2 = parser2.run();
    SyntaxNode *expr = tree2->children[0];
    Value *v = env.eval(expr);
    NumberValue expected(9);
    EXPECT_TRUE(expected.equals(v));
}

TEST(eval, if_statement) {
    auto stringInput = StringInputSource(
            L"関数、五番です（番号）\n"
            L"　もし、イコール（番号、５）\n"
            L"　　返す、１\n"
            L"　返す、０\n"
            L""
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
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
    testTokenizer = InputSourceTokenizer(&stringInput2);
    parser = Parser(&testTokenizer);
    tree = parser.run();
    SyntaxNode *expr = tree->children[0];
    Value *v = env.eval(expr);
    NumberValue expected(1);
    EXPECT_TRUE(expected.equals(v));

    auto stringInput3 = StringInputSource(
            L"五番です（４）"
    );
    testTokenizer = InputSourceTokenizer(&stringInput3);
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
            L"フィボナッチ（１４）\n"
    );


    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    Context context;
    Environment env(&context);
    env.eval(tree->children[0]);

    Value *v = env.eval(tree->children[1]);
    Value *expected = new NumberValue(21);
    EXPECT_TRUE(expected->equals(v));

    v = env.eval(tree->children[2]);
    expected = new NumberValue(610);
    EXPECT_TRUE(expected->equals(v));
}

TEST(program, string_eq) {
    auto stringInput = StringInputSource(
            L"あ＝イコール（「ほげ」、「ほけ」）\n"
            L"い＝イコール（「ほげ」、「ほげ」）\n"
    );


    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    Context context;
    Environment env(&context);
    env.eval(tree);

    EXPECT_FALSE(env.lookup(L"あ")->isTruthy());
    EXPECT_TRUE(env.lookup(L"い")->isTruthy());
}

TEST(program, dictionary) {
    auto stringInput = StringInputSource(
            L"ほげ＝辞書（）\n"
            L"ほげ・あ＝辞書（）\n"
            L"ほげ・あ・い＝辞書（）\n"
            L"関数、ピン（）\n"
            L"　返す、ほげ・あ・い\n"
            L"ほげ・あ・い・う＝ピン\n"
            L"ほげ・あ・い・え＝５\n"
            L"あ＝ほげ・あ・い・う（）・え\n"
    );


    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    Context context;
    Environment env(&context);
    env.eval(tree);

    EXPECT_TRUE(env.lookup(L"あ")->equals(new NumberValue(5)));
}

TEST(program, if_elif_else) {
    auto stringInput = StringInputSource(
            L"関数、五番です（番号）\n"
            L"　もし、イコール（番号、５）\n"
            L"　　返す、１\n"
            L"　他でもし、イコール（番号、６）\n"
            L"　　返す、６\n"
            L"　その他\n"
            L"　　返す、０\n"
            L"\n"
            L"あ＝五番です（５）\n"
            L"い＝五番です（６）\n"
            L"う＝五番です（７）\n"
    );


    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    Context context;
    Environment env(&context);
    env.eval(tree);

    EXPECT_TRUE(env.lookup(L"あ")->equals(new NumberValue(1)));
    EXPECT_TRUE(env.lookup(L"い")->equals(new NumberValue(6)));
    EXPECT_TRUE(env.lookup(L"う")->equals(new NumberValue(0)));
}

TEST(eval, user_function_varargs) {
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、＊配列引数、＊＊辞書引数）\n"
            L"　返す、１\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *functionValue = env.lookup(L"ほげ");

    EXPECT_EQ(functionValue->type, ValueType::FUNC);
    EXPECT_EQ(((FunctionValue *) functionValue)->functionType,
              FunctionValueType::USER_FUNCTION);
    auto *userFunctionValue = (UserFunctionValue *) functionValue;
    EXPECT_TRUE(userFunctionValue->hasVarKeywordParam());
    EXPECT_EQ(userFunctionValue->getVarKeywordParam(), L"辞書引数");
    EXPECT_TRUE(userFunctionValue->hasVarParam());
    EXPECT_EQ(userFunctionValue->getVarParam(), L"配列引数");
}

TEST(eval, call_user_function_varargs) {
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、＊配列引数、＊＊辞書引数）\n"
            L"　返す、配列引数\n"
            L"あ＝ほげ（１、２、３、４、あ：５）\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *result = env.lookup(L"あ");

    EXPECT_EQ(result->type, ValueType::ARRAY);
    auto arrayValue = (ArrayValue *) result;
    EXPECT_EQ(arrayValue->length(), 3);
    EXPECT_EQ(*arrayValue->get(0)->toNumberValue(), NumberValue(2));
    EXPECT_EQ(*arrayValue->get(1)->toNumberValue(), NumberValue(3));
    EXPECT_EQ(*arrayValue->get(2)->toNumberValue(), NumberValue(4));
}

TEST(eval, infix) {
    auto stringInput = StringInputSource(
            L"あ＝１＋２－４＋３\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *result = env.lookup(L"あ");

    EXPECT_EQ(result->type, ValueType::NUM);
    auto numValue = (NumberValue *) result;
    EXPECT_EQ(numValue->value, 2);
}
