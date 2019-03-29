#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

enum class ValueType {
	NUM, FUNC, NONE, RETURN
};

class NumberValue;

class Value {
public:
	Value(ValueType t) : type(t) {};

	virtual bool equals(Value *rhs) const;

	virtual string toString() const;

	NumberValue *toNumberValue() { return (NumberValue *) this; }

	virtual bool isTruthy() const { return false; };

	ValueType type;
};

class NumberValue : public Value {
public:
	NumberValue(int n) : value(n), Value(ValueType::NUM) {};

	virtual bool equals(Value *rhs) const override;

	virtual string toString() const override;

	virtual bool isTruthy() const override { return value != 0; };

	int value;
};

// Indicates that an expression should force exit of func body eval.
class ReturnValue : public Value {
public:
	Value *inner;
	ReturnValue(Value *inner) : inner(inner), Value(ValueType::RETURN) {};
};

class Environment;

class FunctionValue : public Value {
public:
	FunctionValue() : Value(ValueType::FUNC) {};

	virtual Value *apply(
		vector<Value *> args, Environment *env) const = 0;
};

class SyntaxNode;

class UserFunctionValue : public FunctionValue {
	vector<wstring> params;
	SyntaxNode *body;
public:
	UserFunctionValue(vector<wstring> params, SyntaxNode *body)
	 : FunctionValue(), params(params), body(body) {};

	virtual Value *apply(
		vector<Value *> args, Environment *env) const override;

	virtual string toString() const override;
};

// Builtin Functions
class FunctionSum : public FunctionValue {
public:
	virtual Value *apply(
		vector<Value *> args, Environment *env) const override;
};

class FunctionDiff : public FunctionValue {
public:
	virtual Value *apply(
		vector<Value *> args, Environment *env) const override;
};

class FunctionPrint : public FunctionValue {
public:
	virtual Value *apply(
		vector<Value *> args, Environment *env) const override;
};

class FunctionEqual : public FunctionValue {
public:
	virtual Value *apply(
		vector<Value *> args, Environment *env) const override;
};

#endif