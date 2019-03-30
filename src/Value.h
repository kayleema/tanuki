#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

enum class ValueType {
	NUM, FUNC, NONE, RETURN, STRING, TAIL_CALL
};

class NumberValue;
class StringValue;

class Value {
public:
	Value(ValueType t) : type(t) {
		// cout << "Construct value " << this->toString() << endl;
	};

	virtual ~Value() {}

	virtual bool equals(Value *rhs) const;

	virtual string toString() const;

	NumberValue *toNumberValue() { return (NumberValue *) this; }
	StringValue *toStringValue() { return (StringValue *) this; }

	virtual bool isTruthy() const { return false; };

	ValueType type;
	int refs = 0;
};

class NumberValue : public Value {
public:
	NumberValue(int n) : Value(ValueType::NUM), value(n) {};

	virtual bool equals(Value *rhs) const override;

	virtual string toString() const override;

	virtual bool isTruthy() const override { return value != 0; };

	long value;
};

class StringValue : public Value {
public:
	StringValue(wstring str) : Value(ValueType::STRING), value(str) {};

	// virtual bool equals(Value *rhs) const override;

	virtual string toString() const override;

	// virtual bool isTruthy() const override { return value != 0; };

	wstring value;
};

// Indicates that an expression should force exit of func body eval.
class ReturnValue : public Value {
public:
	Value *inner;
	ReturnValue(Value *inner) : Value(ValueType::RETURN), inner(inner) {};
};

class TailCallValue : public Value {
public:
	vector<Value*> args;
	TailCallValue(vector<Value*> _args)
	 : Value(ValueType::TAIL_CALL), args(_args) {};
};

class Environment;

enum class FunctionValueType {
	NONE, USER_FUNCTION
};

class FunctionValue : public Value {
public:
	FunctionValue() : Value(ValueType::FUNC) {};

	virtual Value *apply(
		vector<Value *> args, Environment *env) const = 0;

	FunctionValueType functionType = FunctionValueType::NONE;
};

class SyntaxNode;

class UserFunctionValue : public FunctionValue {
	vector<wstring> params;
	SyntaxNode *body;
public:
	UserFunctionValue(vector<wstring> params, SyntaxNode *body, 
		Environment *parentEnv)
	 : FunctionValue(), params(params), body(body), parentEnv(parentEnv) {
		functionType = FunctionValueType::USER_FUNCTION;
	 };

	Environment *parentEnv;

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