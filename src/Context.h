#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_set>

#include "Environment.h"

class Context {
    unordered_map<long, NumberValue *> preallocNumbers;
    unordered_set<Value *> values;
    unordered_set<Environment *> environments;

    long iteration = 0;
    long frequency = 1;

public:
    unordered_set<Value *> usedValues;
    unordered_set<Environment *> usedEnvironments;

    Context();

    void mark(Value *value);

    void mark(Environment *current_env);

    void collect(Environment *current_env);

    void cleanup();

    void setFrequency(long freq);

    static Value *newNoneValue();

    NumberValue *newNumberValue(long number);

    StringValue *newStringValue(wstring str);

    DictionaryValue *newDictionaryValue();

    UserFunctionValue *newUserFunctionValue(
            vector<wstring> params, SyntaxNode *body, Environment *e);

    ArrayValue *newArrayValue();

    Environment *newChildEnvironment(Environment *e);
};

#endif
