#ifndef CORE_FUNCTIONS_H
#define CORE_FUNCTIONS_H

#include "Value.h"

class Environment;

void initModule(Environment *env);

extern const wchar_t *coreTanukiStarter;

class FunctionNewDictionary : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *kwargsIn) const override;
};

class FunctionForEach : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *kwargsIn) const override;
};

#endif