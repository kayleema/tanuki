#include <utility>

#include "Context.h"


void Context::mark(Value *value) {
    if (!usedValues.count(value)) {
        usedValues.insert(value);
        if (value->type == ValueType::FUNC &&
            ((FunctionValue *) value)->functionType == FunctionValueType::USER_FUNCTION) {
            auto f = (UserFunctionValue *) value;
            mark(f->parentEnv);
            auto defaults = f->getParamsWithDefault();
            for (const auto &defaultItem : defaults) {
                mark(defaultItem.second);
            }
        }
        if (value->type == ValueType::FUNC &&
            ((FunctionValue *) value)->functionType == FunctionValueType::BOUND_FUNCTION) {
            auto f = dynamic_cast<BoundFunctionValue *> (value);
            mark(f->function);
            mark(f->jibun);
        }
        if ((value->type == ValueType::DICT) || (value->type == ValueType::ARRAY)) {
            auto d = static_cast<DictionaryValue *>(value);
            for (const auto &item : d->value) {
                mark(item.second);
            }
            if (d->parent) {
                mark(d->parent);
            }
        }
        if (value->type == ValueType::ARRAY) {
            auto array = (ArrayValue *) value;
            for (const auto &item : array->value) {
                mark(item);
            }
        }
    }
}

void Context::mark(Environment *current_env) {
    if (usedEnvironments.count(current_env)) {
        return;
    }
    usedEnvironments.insert(current_env);
    if (current_env->parent) {
        mark(current_env->parent);
    }
    if (current_env->caller) {
        mark(current_env->caller);
    }
    for (const auto &binding : current_env->bindings) {
        if (binding.second->type != ValueType::NONE) {
            mark(binding.second);
        }
    }
}

void Context::markTempRefs() {
    for (auto &ref : tempReferences) {
        mark(ref);
    }
}

void Context::tempRefIncrement(Value *value) {
    if (value->type != ValueType::NONE) {
        tempReferences.insert(value);
    }
}

void Context::tempRefDecrement(Value *value) {
    if (value->type != ValueType::NONE) {
        auto it = tempReferences.find(value);
        if (it != tempReferences.end()) {
            tempReferences.erase(it);
        } else {
            cout << "エラー：ワーニング：仮のメモリーが二倍消されました。" << value->toString() << endl;
        }
    }
}

void Context::collect(Environment *current_env) {
    iteration++;
    if ((iteration % frequency != 0)) {
        return;
    }

    usedValues.clear();
    usedEnvironments.clear();
    mark(current_env);
    markTempRefs();
    for (auto *value : values) {
        if (!usedValues.count(value)) {
            if (value->refs ||
                (value->type == ValueType::NUM && preallocNumbers.count(((NumberValue *) value)->value)) ||
                (value->type == ValueType::NONE)) {
                usedValues.insert(value);
            } else {
            }
        } else {
        }
    }
    values = usedValues;
    for (auto *environment : environments) {
        if (!usedEnvironments.count(environment)) {
            delete environment;
        }
    }
    environments = usedEnvironments;
}

void Context::cleanup() {
    for (auto value : values) {
        if (value->type == ValueType::NONE) {
            // None is the only statically allocated type
            continue;
        }
        delete value;
    }
    for (auto environment : environments) {
        delete environment;
    }
}

void Context::setFrequency(long freq) {
    frequency = freq;
}

//TODO: Often accessed through instance.
Value *Context::newNoneValue() {
    static Value staticNone(ValueType::NONE);
    return &staticNone;
}

NumberValue *Context::newNumberValue(long number) {
    if (preallocNumbers.count(number)) {
        return preallocNumbers[number];
    }
    auto result = new NumberValue(number);
    values.insert(result);
    preallocNumbers[number] = result;
    return result;
}

FloatValue *Context::newFloatValue(double number) {
    auto result = new FloatValue(number);
    values.insert(result);
    return result;
}

StringValue *Context::newStringValue(wstring str) {
    auto result = new StringValue(std::move(str));
    values.insert(result);
    return result;
}

DictionaryValue *Context::newDictionaryValue() {
    auto result = new DictionaryValue();
    values.insert(result);
    return result;
}

UserFunctionValue *Context::newUserFunctionValue(
        vector<wstring> params, SyntaxNode *body, Environment *e) {
    auto result = new UserFunctionValue(std::move(params), body, e);
    values.insert(result);
    return result;
}

UserFunctionValue *Context::newUserFunctionValue(
        vector<wstring> params, unordered_map<wstring, Value *> paramsWithDefault,
        SyntaxNode *body, Environment *e) {
    auto result = new UserFunctionValue(std::move(params),
                                        std::move(paramsWithDefault), body, e);
    values.insert(result);
    return result;
}

FunctionValue *Context::newBoundFunctionValue(FunctionValue *function, Value *jibun) {
    auto result = new BoundFunctionValue(function, jibun);
    values.insert(result);
    return result;
}

ArrayValue *Context::newArrayValue(Environment *env) {
    auto result = new ArrayValue();
    values.insert(result);
    result->setParent(static_cast<DictionaryValue *>(env->lookup(L"配列型")));
    return result;
}

Environment *Context::newChildEnvironment(Environment *e) {
    auto result = new Environment(e);
    environments.insert(result);
    return result;
}

Context::Context() {
    for (long i = 0; i < 255; i++) {
        auto numberValue = newNumberValue(i);
        numberValue->refs++;
        preallocNumbers[i] = numberValue;
    }
}
