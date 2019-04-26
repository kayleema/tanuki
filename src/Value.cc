#include <utility>

#include <iostream>

#include "Value.h"
#include "Evaluate.h"
#include "Context.h"

bool Value::equals(Value *rhs) const {
    return (type == rhs->type);
}

bool NumberValue::equals(Value *rhs) const {
    return Value::equals(rhs) && (value == ((NumberValue *) rhs)->value);
}

string Value::toString() const {
    ostringstream result;
    result << "Value(";
    result << ValueTypeStrings[(int) type];
    result << ")";
    return result.str();
}

string NumberValue::toString() const {
    ostringstream result;
    result << "NumberValue(" << value << ")";
    return result.str();
}

string StringValue::toString() const {
    return string("string");
}

bool StringValue::equals(Value *rhs) const {
    return Value::equals(rhs) && (value == ((StringValue *) (rhs))->value);
}

string DictionaryValue::toString() const {
    ostringstream result;
    result << "DictionaryValue(" << value.size() << ")";
    return result.str();
}

DictionaryValue::DictionaryValue() : Value(ValueType::DICT), parent(nullptr) {}

string UserFunctionValue::toString() const {
    ostringstream result;
    result << "UserFunctionValue(" << "..." << ")";
    return result.str();
}

Value *UserFunctionValue::apply(const vector<Value *> &argsIn,
                                Environment *caller, unordered_map<wstring, Value *> *kwargsIn) const {
    Value *bodyReturnValue;
    Environment *env;
    env = parentEnv->newChildEnvironment();
    env->caller = caller;
    vector<Value *> args = argsIn;
    do {
        if (params.size() > argsIn.size()) {
            cout << "エラー：引数は足りません　"
                 << "必要は" << params.size() << " 渡したのは" << argsIn.size() << endl;
        }
        for (size_t i = 0; i < params.size(); i++) {
            env->bind(params[i], args[i]);
        }
        if (hasVarKeywordArgs) {
            auto kwArgs = env->context->newDictionaryValue();
            if (kwargsIn) {
                for (const auto &arg : *kwargsIn) {
                    kwArgs->set(arg.first, arg.second);
                }
            }
            env->bind(varKeywordArgsParam, kwArgs);
        }
        if (hasVarArgs) {
            auto varArgs = env->context->newArrayValue();
            if (argsIn.size() > params.size()) {
                for (size_t i = params.size(); i < argsIn.size(); i++) {
                    varArgs->push(argsIn[i]);
                }
            }
            env->bind(varArgsParam, varArgs);
        }
        bodyReturnValue = env->eval(body, this);
        if (bodyReturnValue->type == ValueType::RETURN) {
            auto inner = ((ReturnValue *) bodyReturnValue)->inner;
            delete bodyReturnValue;
            return inner;
        }
        if (bodyReturnValue->type == ValueType::TAIL_CALL) {
            args = ((TailCallValue *) bodyReturnValue)->args;
            delete bodyReturnValue;
            env->tailReset();
        }
    } while (bodyReturnValue->type == ValueType::TAIL_CALL);
    return bodyReturnValue;
}

void UserFunctionValue::setVarKeywordParam(wstring name) {
    hasVarKeywordArgs = true;
    varKeywordArgsParam = std::move(name);
}

void UserFunctionValue::setVarParam(wstring name) {
    hasVarArgs = true;
    varArgsParam = std::move(name);
}

string ArrayValue::toString() const {
    stringstream ss;
    ss << "Array(長さ=" << length() << ")";
    return ss.str();
}
