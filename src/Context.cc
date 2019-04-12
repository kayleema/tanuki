#include "Context.h"


void Context::mark(Value *value) {
	if (!usedValues.count(value)) {
		usedValues.insert(value);
		if (value->type == ValueType::FUNC &&
			((FunctionValue *)value)->functionType == FunctionValueType::USER_FUNCTION) {
			auto f = (UserFunctionValue *)value;
			mark(f->parentEnv);
		}
		if (value->type == ValueType::DICT) {
			auto d = (DictionaryValue *)value;
			for (auto item : d->value) {
				mark(item.second);
			}
			if (d->parent) {
				mark(d->parent);
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
	for (auto binding : current_env->bindings) {
		mark(binding.second);
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
	for (auto *value : values) {
		if (!usedValues.count(value)) {
			if (value->refs){
				usedValues.insert(value);
			} else {
				delete value;
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

void Context::setFrequency(int freq) {
	frequency = freq;
}

Value *Context::newNoneValue() {
	static Value staticNone(ValueType::NONE);
	return &staticNone;
}

NumberValue *Context::newNumberValue(long number) {
	auto result = new NumberValue(number);
	values.insert(result);
	return result;
}

StringValue *Context::newStringValue(wstring str) {
	auto result = new StringValue(str);
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
	auto result = new UserFunctionValue(params, body, e);
	values.insert(result);
	return result;
}

void Context::trackValue(Value *value) {
	values.insert(value);
}

Environment *Context::newChildEnvironment(Environment *e) {
	auto result = new Environment(e);
	environments.insert(result);
	return result;
}
