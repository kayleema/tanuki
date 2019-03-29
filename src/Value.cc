#include <iostream>

#include "Value.h"

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


Value *FunctionSum::apply(vector<Value *> args) const {
	int result = 0;
	for (auto value : args) {
		result += value->toNumberValue()->value;
	}
	return new NumberValue(result);
};

Value *FunctionDiff::apply(vector<Value *> args) const {
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

Value *FunctionPrint::apply(vector<Value *> args) const {
	for (auto value : args) {
		cout << value->toNumberValue()->value << endl;
	}
	return new Value(ValueType::NONE);
};
