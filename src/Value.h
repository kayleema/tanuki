#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

enum class ValueType {
	NUM, FUNC, NONE
};

class NumberValue;

class Value {
public:
	Value(ValueType t) : type(t) {};

	virtual bool equals(Value *rhs) const;

	virtual string toString() const;

	NumberValue *toNumberValue() { return (NumberValue *) this; }

	ValueType type;
};

class NumberValue : public Value {
public:
	NumberValue(int n) : value(n), Value(ValueType::NUM) {};

	virtual bool equals(Value *rhs) const;

	virtual string toString() const;

	int value;
};

class FunctionValue : public Value {
public:
	FunctionValue() : Value(ValueType::FUNC) {};

	virtual Value *apply(vector<Value *> args) const = 0;
};

// Builtin Functions
class FunctionSum : public FunctionValue {
public:
	virtual Value *apply(vector<Value *> args) const;
};

class FunctionDiff : public FunctionValue {
public:
	virtual Value *apply(vector<Value *> args) const;
};

class FunctionPrint : public FunctionValue {
public:
	virtual Value *apply(vector<Value *> args) const;
};

#endif