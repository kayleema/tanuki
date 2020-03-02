#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_set>

#include "Environment.h"

// Manages Memory and object lifecycle
class Context {
    unordered_map<long, NumberValue *> preallocNumbers;
    unordered_set<Value *> values;
    unordered_set<Environment *> environments;
    unordered_multiset<Value *> tempReferences;

    long iteration = 0;
    long frequency = 1;

public:
    unordered_set<Value *> usedValues;
    unordered_set<Environment *> usedEnvironments;

    Context();

    void tempRefIncrement(Value *value);

    void tempRefDecrement(Value *value);

    void mark(Value *value);

    void mark(Environment *current_env);

    void markTempRefs();

    void collect(Environment *current_env);

    void cleanup();

    void setFrequency(long freq);

    static Value *newNoneValue();

    NumberValue *newNumberValue(long number);

    StringValue *newStringValue(wstring str);

    DictionaryValue *newDictionaryValue();

    UserFunctionValue *newUserFunctionValue(
            vector<wstring> params, SyntaxNode *body, Environment *e);

    UserFunctionValue *newUserFunctionValue(
            vector<wstring> params,
            unordered_map<wstring, Value *> paramsWithDefault,
            SyntaxNode *body, Environment *e);

    FunctionValue *newBoundFunctionValue(FunctionValue *function, Value *jibun);

    ArrayValue *newArrayValue();

    Environment *newChildEnvironment(Environment *e);
};

#endif
