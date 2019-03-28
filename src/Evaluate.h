#ifndef EVALUATE_H
#define EVALUATE_H

#include <unordered_map>
#include "Parser.h"

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

class FunctionSum : public FunctionValue {
public:
	virtual Value *apply(vector<Value *> args) const {
		int result = 0;
		for (auto value : args) {
			result += value->toNumberValue()->value;
		}
		return new NumberValue(result);
	};
};

class FunctionDiff : public FunctionValue {
public:
	virtual Value *apply(vector<Value *> args) const {
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
};

class FunctionPrint : public FunctionValue {
public:
	virtual Value *apply(vector<Value *> args) const {
		for (auto value : args) {
			cout << value->toNumberValue()->value << endl;
		}
		return new Value(ValueType::NONE);
	};
};

class Environment {
public:
	Environment() {
		bindings[L"足す"] = new FunctionSum();
		bindings[L"引く"] = new FunctionDiff();
		bindings[L"表示"] = new FunctionPrint();
	}

	unordered_map<wstring, Value*> bindings;

	Value *eval(SyntaxNode *node);
};

#endif