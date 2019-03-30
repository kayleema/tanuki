#include <iostream>

#include "Value.h"
#include "Evaluate.h"

bool Value::equals(Value *rhs) const{
	return (type == rhs->type);
}

bool NumberValue::equals(Value *rhs) const{
	return Value::equals(rhs) && (value == ((NumberValue *)rhs)->value);
}

string Value::toString() const{
	ostringstream result;
	result << "Value(";
	result << vector<string>({"NUM", "FUNC", "NONE", "RETURN"})[(int)type];
	result << ")";
	return result.str();
}
string NumberValue::toString() const{
	ostringstream result;
	result << "NumberValue(" << value << ")";
	return result.str();
}

string UserFunctionValue::toString() const {
	ostringstream result;
	result << "UserFunctionValue(" << "..." << ")";
	return result.str();
}
Value *UserFunctionValue::apply(
		vector<Value *> args, Environment *caller) const {
	Environment *env = parentEnv->newChildEnvironment();
	env->caller = caller;
	for (size_t i = 0; i < args.size(); i++) {
		env->bind(params[i],args[i]);
	}
	auto bodyReturnValue = env->eval(body);
	if (bodyReturnValue->type == ValueType::RETURN) {
		auto inner = ((ReturnValue*)bodyReturnValue)->inner;
		delete bodyReturnValue;
		return inner;
	}
	return bodyReturnValue;
}

Value *FunctionSum::apply(
		vector<Value *> args, Environment *env) const {
	int result = 0;
	for (auto value : args) {
		result += value->toNumberValue()->value;
	}
	return env->context->newNumberValue(result);
};

Value *FunctionDiff::apply(
		vector<Value *> args, Environment *env) const {
	int result = 0;
	bool first = true;
	for (auto value : args) {
		if (first) {
			first = false;
			result = value->toNumberValue()->value;
		} else {
			result -= value->toNumberValue()->value;
		}
	}
	return env->context->newNumberValue(result);
};

Value *FunctionPrint::apply(
		vector<Value *> args, Environment *env) const {
	for (auto value : args) {
		cout << value->toNumberValue()->value << endl;
	}
	return env->context->newNoneValue();
};

Value *FunctionEqual::apply(
		vector<Value *> args, Environment *env) const {
	if(args[0]->equals(args[1])) {
		return env->context->newNumberValue(1);
	}
	return env->context->newNumberValue(0);
};
