#include <iostream>

#include "Value.h"
#include "Evaluate.h"
#include "Context.h"

bool Value::equals(Value *rhs) const{
	return (type == rhs->type);
}

bool NumberValue::equals(Value *rhs) const{
	return Value::equals(rhs) && (value == ((NumberValue *)rhs)->value);
}

string Value::toString() const{
	ostringstream result;
	result << "Value(";
	result << vector<string>({"NUM", "FUNC", "NONE", "RETURN", "STRING", "TAIL_CALL", "DICT"})[(int)type];
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

string DictionaryValue::toString() const{
	ostringstream result;
	result << "DictionaryValue(" << value.size() << ")";
	return result.str();
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
			args = ((TailCallValue *)bodyReturnValue)->args;
			delete bodyReturnValue;
		}
	} while(bodyReturnValue->type == ValueType::TAIL_CALL);
	return bodyReturnValue;
}
