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
        if (value->type == ValueType::DICT) {
            auto d = (DictionaryValue *) value;
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

void Context::tempRefIncrement(Value *value){
    tempReferences.insert(value);
}

void Context::tempRefDecrement(Value *value){
    auto it = tempReferences.find(value);
    if (it != tempReferences.end()) {
        // TODO: Garbage collector bug with extra decrements that don't match increments.
        tempReferences.erase(it);
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
            if (value->refs) {
                usedValues.insert(value);
            } else {
                if (value->type == ValueType::NUM &&
                    preallocNumbers.count(((NumberValue *) value)->value)) {
                    // Don't deallocate numbers saved in prealloc
                    // This used to be here: preallocNumbers.erase(((NumberValue *) value)->value);
                } else if (value->type == ValueType::NONE) {
                    // Don't deallocate none type;
                } else {
                    delete value;
                }
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

ArrayValue *Context::newArrayValue() {
    auto result = new ArrayValue();
    values.insert(result);
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
