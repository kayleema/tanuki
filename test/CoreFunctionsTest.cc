#include "Runtime/CoreFunctions.h"
#include "gtest/gtest.h"

#include "Lexer/Tokenizer.h"
#include "Runtime/Environment.h"
#include "Runtime/Context.h"
#include <fakeit.hpp>
#include "MockFunction.h"

using namespace fakeit;

TEST(coreFunctions, functionNewDictionary) {
    Context context;
    Environment env(&context);
    FunctionNewDictionary f;
    unordered_map<string, Value *> kwargs(
        {
            {"hello", context.newNumberValue(7)}
        });
    auto result = f.apply({}, &env, &kwargs);

    EXPECT_EQ(ValueType::DICT, result->type);
    auto dict = result->toDictionaryValue();
    EXPECT_EQ(dict->parent, nullptr);
    EXPECT_EQ(dict->get("hello"), context.newNumberValue(7));

    context.cleanup();
}

TEST(coreFunctions, functionForEachString) {
    Context context;
    Environment env(&context);
    FunctionForEach f;
    MockFunction mockFunction;
    Mock<MockFunction> spy(mockFunction);
    Spy(Method(spy, apply));

    EXPECT_EQ(ValueType::FUNC, mockFunction.type);

    unordered_map<string, Value *> kwargs;
    vector<Value*> args;
    args.push_back(context.newStringValue("fooほげfoo"));
    args.push_back(&mockFunction);
    auto result = f.apply(args, &env, &kwargs);

    EXPECT_EQ(ValueType::NONE, result->type);
    Verify(Method(spy,apply)).Exactly(8);
    context.cleanup();
}

TEST(coreFunctions, functionLengthString) {
    Context context;
    Environment env(&context);
    LengthFunction f;

    EXPECT_EQ(ValueType::NUM,
              f.apply({context.newStringValue("fooほげfoo")}, &env, {})
                  ->toNumberValue()
                  ->type);
    EXPECT_EQ(8, f.apply({context.newStringValue("fooほげfoo")}, &env, {})
                     ->toNumberValue()
                     ->value);
    EXPECT_EQ(11, f.apply({context.newStringValue("spamandeggs")}, &env, {})
                      ->toNumberValue()
                      ->value);
    EXPECT_EQ(3, f.apply({context.newStringValue("🤣🤣😂")}, &env, {})
                     ->toNumberValue()
                     ->value);
    EXPECT_EQ(7, f.apply({context.newStringValue("美味しい𰻞𰻞麺")}, &env, {})
                     ->toNumberValue()
                     ->value);
    context.cleanup();
}
