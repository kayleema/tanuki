#include <utility>

#include <iostream>

#include "Value.h"
#include "Environment.h"
#include "Context.h"
#include "Logger.h"

bool Value::equals(const Value *rhs) const {
    return (type == rhs->type);
}

DictionaryValue *Value::getLookupSource(Environment *) {
    return nullptr;
}

DictionaryValue *NumberValue::getLookupSource(Environment *env) {
    return dynamic_cast<DictionaryValue *>(env->lookup(L"番号型"));
}

DictionaryValue *FloatValue::getLookupSource(Environment *env) {
    return dynamic_cast<DictionaryValue *>(env->lookup(L"番号フロート型"));
}

DictionaryValue *StringValue::getLookupSource(Environment *env) {
    return dynamic_cast<DictionaryValue *>(env->lookup(L"文字列型"));
}

DictionaryValue *DictionaryValue::getLookupSource(Environment *) {
    return this;
}

DictionaryValue *FunctionValue::getLookupSource(Environment *env) {
    return dynamic_cast<DictionaryValue *>(env->lookup(L"関数型"));
}

string FunctionValue::toStringJP() const {
    stringstream ss;
    ss << "関数値";
    return ss.str();
}

bool NumberValue::equals(const Value *rhs) const {
    return Value::equals(rhs) && (value == ((NumberValue *) rhs)->value);
}

string Value::toString() const {
    ostringstream result;
    result << "Value(";
    result << ValueTypeStrings[(int) type];
    result << ")";
    return result.str();
}

string Value::toStringJP() const {
    ostringstream result;
    result << "無";
    return result.str();
}

bool operator==(const Value &lhs, const Value &rhs) {
    return lhs.equals(&rhs);
}

string NumberValue::toString() const {
    ostringstream result;
    result << "NumberValue(" << value << ")";
    return result.str();
}

string NumberValue::toStringJP() const {
    ostringstream result;
    result << value;
    return result.str();
}

bool FloatValue::equals(const Value *rhs) const {
    static const double EPSILON = 0.00001;
    return Value::equals(rhs) && ( abs(value - ((FloatValue *) rhs)->value) < EPSILON);
}

string FloatValue::toString() const {
    ostringstream result;
    result << "FloatValue(" << value << ")";
    return result.str();
}

string FloatValue::toStringJP() const {
    ostringstream result;
    result << value;
    return result.str();
}

string StringValue::toString() const {
    return string("string");
}

string StringValue::toStringJP() const {
    ostringstream result;
    result << "「" << encodeUTF8(value) << "」";
    return result.str();
}

bool StringValue::equals(const Value *rhs) const {
    return Value::equals(rhs) && (value == ((const StringValue *) (rhs))->value);
}

string DictionaryValue::toString() const {
    ostringstream result;
    result << "DictionaryValue(" << value.size() << ")";
    return result.str();
}

string DictionaryValue::toStringJP() const {
    ostringstream result;
    result << "辞書〈長さ：" << value.size() << "〉";
    return result.str();
}

DictionaryValue::DictionaryValue() : Value(ValueType::DICT), parent(nullptr) {}

bool DictionaryValue::equals(const Value *rhs) const {
    return this == rhs;
}

string UserFunctionValue::toString() const {
    ostringstream result;
    result << "UserFunctionValue(" << "..." << ")";
    return result.str();
}

Value *UserFunctionValue::apply(const vector<Value *> &argsIn,
                                Environment *caller, unordered_map<wstring, Value *> *kwargsIn) const {
    Value *bodyReturnValue = nullptr;
    Environment *env;
    env = parentEnv->newChildEnvironment();
    env->caller = caller;
    vector<Value *> args;
    args.insert(args.end(), argsIn.begin(), argsIn.end());
    unordered_map<wstring, Value *> kwArgsStatic;
    if (kwargsIn != nullptr) {
        kwArgsStatic = *kwargsIn;
    }
    unordered_map<wstring, Value *> *kwArgs = (kwargsIn == nullptr) ? nullptr : (&kwArgsStatic);
    do {
        if (bodyReturnValue != nullptr) {
            delete bodyReturnValue;
        }

        if (params.size() > argsIn.size()) {
            // TODO: also log original function name
            ConsoleLogger().log("エラー：引数は足りません　")
                    ->log("必要は")->logLong((long) params.size())
                    ->log(" 渡したのは")->logLong((long) argsIn.size())
                    ->logEndl();
            return env->context->newNoneValue();
        }

        // bind normal params
        for (size_t i = 0; i < params.size(); i++) {
            env->bind(params[i], args[i]);
        }

        // bind default parameters when not specified
        for (auto item : paramsWithDefault) {
            wstring left = item.first;
            if (kwArgs && kwArgs->count(item.first)) {
                env->bind(item.first, (*kwArgs)[item.first]);
            } else {
                env->bind(item.first, item.second);
            }
        }
        if (hasVarKeywordArgs) {
            auto kwArgsDict = env->context->newDictionaryValue();
            if (kwArgs) {
                for (const auto &arg : *kwArgs) {
                    kwArgsDict->set(arg.first, arg.second);
                }
            }
            env->bind(varKeywordArgsParam, kwArgsDict);
        }
        if (hasVarArgs) {
            auto varArgs = env->context->newArrayValue(env);
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
            TailCallValue *tailCallValue = (TailCallValue *) bodyReturnValue;
            args = tailCallValue->args;
            if (tailCallValue->hasKwArgs) {
                if (!kwArgs) {
                    kwArgs = &kwArgsStatic;
                }
                kwArgs->clear();
                *kwArgs = tailCallValue->kwArgs;
            } else {
                kwArgs = nullptr;
            }
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

string ArrayValue::toStringJP() const {
    stringstream ss;
    ss << "配列〈長さ：" << length() << "〉";
    return ss.str();
}

Value *BoundFunctionValue::apply(const vector<Value *> &args, Environment *env,
                                 unordered_map<wstring, Value *> *kwargsIn) const {
    vector<Value *> newArgs;
    newArgs.push_back(jibun);
    newArgs.insert(newArgs.end(), args.begin(), args.end());
    return function->apply(newArgs, env, kwargsIn);
}
