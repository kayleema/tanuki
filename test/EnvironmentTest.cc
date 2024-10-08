#include <Logger.h>
#include "gtest/gtest.h"
#include "fakeit.hpp"
#include "Lexer/Tokenizer.h"
#include "Parser/Parser.h"
#include "Runtime/Environment.h"
#include "Runtime/Context.h"
#include "TextInput/StringInputSource.h"

using namespace fakeit;

TEST(eval, functions) {
    auto stringInput = StringInputSource(
            "足す（２、引く（５、２）、４）"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    SyntaxNode *expr = tree->children[0];
    string treeString = expr->toString();

    Context context;
    auto *env = new Environment(&context);
    Value *v = env->eval(expr);
    NumberValue expected(9);

    EXPECT_TRUE(expected.equals(v));

    context.cleanup();
}

TEST(eval, floatNumber) {
    // Todo: fuzz and performance test this
    auto stringInput = StringInputSource(
            "１２。３４"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    SyntaxNode *expr = tree->children[0];
    string treeString = expr->toString();

    Context context;
    auto *env = new Environment(&context);
    Value *v = env->eval(expr);
    FloatValue expected(12.34);

    EXPECT_TRUE(expected.equals(v));

    context.cleanup();
}

TEST(eval, import_statement) {
    Mock<Filesystem> mockFilesystem;
    When(Method(mockFilesystem, getInputSourceForFilename)).Do(
            [](const string &) {
                return unique_ptr<InputSource>(new StringInputSource(
                        "あ＝７\n"
                ));
            }
    );

    auto stringInput = StringInputSource(
            "導入、フォルダー・フォルダー２・ファイル名"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    SyntaxNode *expr = tree->children[0];
    string treeString = expr->toString();

    Context context;
    auto *env = new Environment(&context, &mockFilesystem.get());
    env->bind("FILE", context.newStringValue(""));
    env->eval(expr);

    auto value = env
            ->lookup("ファイル名")->toDictionaryValue()
            ->get("あ");
    EXPECT_EQ(ValueType::NUM, value->type);
    EXPECT_EQ(7, value->toNumberValue()->value);

    context.cleanup();
}

TEST(eval, user_function) {
    auto stringInput = StringInputSource(
            "関数、プラス二（あ）\n"
            "　返す、足す（あ、２）\n"
            ""
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *functionValue = env.lookup("プラス二");
    EXPECT_EQ(ValueType::FUNC, functionValue->type);

    auto stringInput2 = StringInputSource(
            "プラス二（７）"
    );
    auto testTokenizer2 = TanukiTokenizer(&stringInput2);
    auto parser2 = Parser(nullptr);
    SyntaxNode *tree2 = parser2.run(testTokenizer2.getAllTokens());
    SyntaxNode *expr = tree2->children[0];
    Value *v = env.eval(expr);
    NumberValue expected(9);
    EXPECT_TRUE(expected.equals(v));
}

TEST(eval, if_statement) {
    auto stringInput = StringInputSource(
            "関数、五番です（番号）\n"
            "　もし、イコール（番号、５）\n"
            "　　返す、１\n"
            "　返す、０\n"
            ""
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *functionValue = env.lookup("五番です");
    EXPECT_EQ(ValueType::FUNC, functionValue->type);

    auto stringInput2 = StringInputSource(
            "五番です（５）"
    );
    auto testTokenizer2 = TanukiTokenizer(&stringInput2);
    parser = Parser(nullptr);
    tree = parser.run(testTokenizer2.getAllTokens());
    SyntaxNode *expr = tree->children[0];
    Value *v = env.eval(expr);
    NumberValue expected(1);
    EXPECT_TRUE(expected.equals(v));

    auto stringInput3 = StringInputSource(
            "五番です（４）"
    );
    auto testTokenizer3 = TanukiTokenizer(&stringInput3);
    parser = Parser(nullptr);
    tree = parser.run(testTokenizer3.getAllTokens());
    expr = tree->children[0];
    v = env.eval(expr);
    expected = NumberValue(0);
    EXPECT_TRUE(expected.equals(v));
}

TEST(program, fibonacci) {
    // Commented out parts are commented out so that tests run faster without optimizations enabled
    auto stringInput = StringInputSource(
            "関数、フィボナッチ（号）\n"
            "　もし、イコール（号、１）\n"
            "　　返す、１\n"
            "　もし、イコール（号、０）\n"
            "　　返す、１\n"
            "　返す、足す（フィボナッチ（引く（号、１））、フィボナッチ（引く（号、２）））\n"
            "\n"
            "フィボナッチ（７）\n"
            // "フィボナッチ（１４）\n"
    );


    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree->children[0]);

    Value *v = env.eval(tree->children[1]);
    Value *expected = new NumberValue(21);
    EXPECT_TRUE(expected->equals(v));

    // v = env.eval(tree->children[2]);
    // expected = new NumberValue(610);
    // EXPECT_TRUE(expected->equals(v));
}

TEST(program, string_eq) {
    auto stringInput = StringInputSource(
            "あ＝イコール（「ほげ」、「ほけ」）\n"
            "い＝イコール（「ほげ」、「ほげ」）\n"
    );


    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);

    EXPECT_FALSE(env.lookup("あ")->isTruthy());
    EXPECT_TRUE(env.lookup("い")->isTruthy());
}

TEST(program, dictionary) {
    auto stringInput = StringInputSource(
            "ほげ＝辞書（）\n"
            "ほげ・あ＝辞書（）\n"
            "ほげ・あ・い＝辞書（）\n"
            "関数、ピン（）\n"
            "　返す、ほげ・あ・い\n"
            "ほげ・あ・い・う＝ピン\n"
            "ほげ・あ・い・え＝５\n"
            "あ＝ほげ・あ・い・う（）・え\n"
    );


    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);

    EXPECT_TRUE(env.lookup("あ")->equals(new NumberValue(5)));
}

TEST(program, if_elif_else) {
    auto stringInput = StringInputSource(
            "関数、五番です（番号）\n"
            "　もし、イコール（番号、５）\n"
            "　　返す、１\n"
            "　あるいは、イコール（番号、６）\n"
            "　　返す、６\n"
            "　その他\n"
            "　　返す、０\n"
            "\n"
            "あ＝五番です（５）\n"
            "い＝五番です（６）\n"
            "う＝五番です（７）\n"
    );


    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);

    EXPECT_TRUE(env.lookup("あ")->equals(new NumberValue(1)));
    EXPECT_TRUE(env.lookup("い")->equals(new NumberValue(6)));
    EXPECT_TRUE(env.lookup("う")->equals(new NumberValue(0)));
}

TEST(eval, user_function_varargs) {
    auto stringInput = StringInputSource(
            "関数、ほげ（引数、＊配列引数、＊＊辞書引数）\n"
            "　返す、１\n"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *functionValue = env.lookup("ほげ");

    EXPECT_EQ(functionValue->type, ValueType::FUNC);
    EXPECT_EQ(((FunctionValue *) functionValue)->functionType,
              FunctionValueType::USER_FUNCTION);
    auto *userFunctionValue = (UserFunctionValue *) functionValue;
    EXPECT_TRUE(userFunctionValue->hasVarKeywordParam());
    EXPECT_EQ(userFunctionValue->getVarKeywordParam(), "辞書引数");
    EXPECT_TRUE(userFunctionValue->hasVarParam());
    EXPECT_EQ(userFunctionValue->getVarParam(), "配列引数");
}

TEST(eval, call_user_function_varargs) {
    auto stringInput = StringInputSource(
            "関数、ほげ（引数、＊配列引数、＊＊辞書引数）\n"
            "　返す、配列引数\n"
            "あ＝ほげ（１、２、３、４、あ：５）\n"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);
//    Value *result = env.lookup("あ");

//    EXPECT_EQ(result->type, ValueType::ARRAY);
//    auto arrayValue = (ArrayValue *) result;
//    EXPECT_EQ(arrayValue->length(), 3);
//    EXPECT_EQ(*arrayValue->getIndex(0)->toNumberValue(), NumberValue(2));
//    EXPECT_EQ(*arrayValue->getIndex(1)->toNumberValue(), NumberValue(3));
//    EXPECT_EQ(*arrayValue->getIndex(2)->toNumberValue(), NumberValue(4));
}

TEST(eval, call_user_function_default) {
    auto stringInput = StringInputSource(
            "関数、ほげ（あああ：１＋２）\n"
            "　返す、あああ\n"
            "あ＝ほげ（あああ：５）\n"
            "い＝ほげ（）\n"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *resultSpecified = env.lookup("あ");
    Value *resultDefault = env.lookup("い");

    EXPECT_EQ(resultSpecified->toNumberValue()->value, 5);
    EXPECT_EQ(*resultDefault->toNumberValue(), NumberValue(3));
}

TEST(eval, infix) {
    auto stringInput = StringInputSource(
            "あ＝１＋２－４＋３\n"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    Environment env(&context);
    env.eval(tree);
    Value *result = env.lookup("あ");

    EXPECT_EQ(result->type, ValueType::NUM);
    auto numValue = (NumberValue *) result;
    EXPECT_EQ(numValue->value, 2);
}

TEST(eval, infix_equality) {
    auto stringInput = StringInputSource(
            "あ＝１＝＝３\n"
            "い＝１！＝３\n"
            "う＝１＞＝３\n"
            "え＝１＜＝３\n"
            "お＝１＞３\n"
            "か＝１＜３\n"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    EXPECT_TRUE(parser.isComplete());
    Context context;
    auto *env = new Environment(&context);
    env->eval(tree);
    Value *result = env->lookup("あ");

    EXPECT_EQ(result->type, ValueType::NUM);
    auto numValue = (NumberValue *) result;
    EXPECT_EQ(numValue->value, 0);
    EXPECT_FALSE(numValue->isTruthy());

    EXPECT_TRUE(env->lookup("い")->isTruthy());
    EXPECT_FALSE(env->lookup("う")->isTruthy());
    EXPECT_TRUE(env->lookup("え")->isTruthy());
    EXPECT_FALSE(env->lookup("お")->isTruthy());
    EXPECT_TRUE(env->lookup("か")->isTruthy());
    context.cleanup();
}

TEST(eval, add) {
    auto stringInput = StringInputSource(
        "あ＝７＋９\n"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(testTokenizer.getAllTokens());
    Context context;
    auto *env = new Environment(&context);
    env->eval(tree);
    EXPECT_EQ(env->lookup("あ")->type, ValueType::NUM);
    context.cleanup();
}
