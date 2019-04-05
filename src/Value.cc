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
	result << vector<string>({"NUM", "FUNC", "NONE", "RETURN", "STRING", "TAIL_CALL"})[(int)type];
	result << ")";
	return result.str();
}
string NumberValue::toString() const{
	ostringstream result;
	result << "NumberValue(" << value << ")";
	return result.str();
}
string StringValue::toString() const{
	return string("string");
}

bool StringValue::equals(Value *rhs) const{
	return Value::equals(rhs) && (value == ((StringValue*)(rhs))->value);
}

string UserFunctionValue::toString() const {
	ostringstream result;
	result << "UserFunctionValue(" << "..." << ")";
	return result.str();
}
Value *UserFunctionValue::apply(
		vector<Value *> args, Environment *caller) const {
	Value *bodyReturnValue;
	Environment *env;
	do {
		env = parentEnv->newChildEnvironment();
		env->caller = caller;

		for (size_t i = 0; i < args.size(); i++) {
			env->bind(params[i],args[i]);
		}
		bodyReturnValue = env->eval(body, this);
		if (bodyReturnValue->type == ValueType::RETURN) {
			auto inner = ((ReturnValue*)bodyReturnValue)->inner;
			delete bodyReturnValue;
			return inner;
		}
		if (bodyReturnValue->type == ValueType::TAIL_CALL) {
			//cout << "tailing" << endl;
			args = ((TailCallValue *)bodyReturnValue)->args;
			delete bodyReturnValue;
		}
	} while(bodyReturnValue->type == ValueType::TAIL_CALL);
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
		if (value->type == ValueType::NUM) {
			cout << value->toNumberValue()->value << endl;
		} else if (value->type == ValueType::STRING) {
			cout << encodeUTF8(value->toStringValue()->value) << endl;
		}
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
