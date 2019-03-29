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
	return "generic value";
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
		vector<Value *> args, Environment *parent) const {
	Environment *env = new Environment(parent);
	for (int i = 0; i < args.size(); i++) {
		env->bind(params[i],args[i]);
	}
	auto bodyReturnValue = env->eval(body);
	if (bodyReturnValue->type == ValueType::RETURN) {
		return ((ReturnValue*)bodyReturnValue)->inner;
	}
	return bodyReturnValue;
}

Value *FunctionSum::apply(
		vector<Value *> args, Environment *env) const {
	int result = 0;
	for (auto value : args) {
		result += value->toNumberValue()->value;
	}
	return new NumberValue(result);
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
	return new NumberValue(result);
};

Value *FunctionPrint::apply(
		vector<Value *> args, Environment *env) const {
	for (auto value : args) {
		cout << value->toNumberValue()->value << endl;
	}
	return new Value(ValueType::NONE);
};

Value *FunctionEqual::apply(
		vector<Value *> args, Environment *env) const {
	if(args[0]->equals(args[1])) {
		return new NumberValue(1);
	}
	return new NumberValue(0);
};
