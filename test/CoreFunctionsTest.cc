#include <CoreFunctions.h>
#include "gtest/gtest.h"

#include "Lexer/Tokenizer.h"
#include "Parser.h"
#include "Environment.h"
#include "Context.h"

TEST(coreFunctions, functionNewDictionary) {
    Context context;
    Environment env(&context);
    FunctionNewDictionary f;
    unordered_map<wstring, Value *> kwargs(
            {
                    {wstring(L"hello"), context.newNumberValue(7)}
            });
    auto result = f.apply({}, &env, &kwargs);

    EXPECT_EQ(ValueType::DICT, result->type);
    auto dict = result->toDictionaryValue();
    EXPECT_EQ(dict->parent, nullptr);
    EXPECT_EQ(dict->get(L"hello"), context.newNumberValue(7));

    context.cleanup();
}
