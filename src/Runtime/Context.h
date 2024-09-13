#ifndef CONTEXT_H
#define CONTEXT_H

#include "Environment.h"
#include <unordered_set>

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

    FloatValue *newFloatValue(double number);

    StringValue *newStringValue(string str);

    DictionaryValue *newDictionaryValue();

    UserFunctionValue *newUserFunctionValue(
            vector<string> params, SyntaxNode *body, Environment *e);

    UserFunctionValue *newUserFunctionValue(
            vector<string> params,
            unordered_map<string, Value *> paramsWithDefault,
            SyntaxNode *body, Environment *e);

    FunctionValue *newBoundFunctionValue(FunctionValue *function, Value *jibun);

    ArrayValue *newArrayValue(Environment *env);

    Environment *newChildEnvironment(Environment *e);
};

#endif
