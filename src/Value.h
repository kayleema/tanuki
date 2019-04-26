#include <utility>

#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

using namespace std;

enum class ValueType {
	NUM, FUNC, NONE, RETURN, STRING, TAIL_CALL, DICT, MODULE
};
const string ValueTypeStrings[] = {
	"NUM", "FUNC", "NONE", "RETURN", "STRING", "TAIL_CALL", "DICT", "MODULE"
};

class NumberValue;
class StringValue;
class DictionaryValue;

class Value {
public:
	explicit Value(ValueType t) : type(t) {
	};

	virtual ~Value() = default;

	virtual bool equals(Value *rhs) const;

	virtual string toString() const;

	NumberValue *toNumberValue() { return (NumberValue *) this; }
	StringValue *toStringValue() { return (StringValue *) this; }
	DictionaryValue *toDictionaryValue() { return (DictionaryValue *) this; }

	virtual bool isTruthy() const { return true; };

	ValueType type;
	int refs = 0;
};

class NumberValue : public Value {
public:
	explicit NumberValue(long n) : Value(ValueType::NUM), value(n) {};

	bool equals(Value *rhs) const override;

	string toString() const override;

	bool isTruthy() const override { return value != 0; };

	long value;
};

class StringValue : public Value {
public:
	explicit StringValue(wstring str) : Value(ValueType::STRING), value(std::move(str)) {};

	bool equals(Value *rhs) const override;

	string toString() const override;

	wstring value;
};

class DictionaryValue : public Value {

public:
	DictionaryValue();
	unordered_map<wstring, Value*> value;
	DictionaryValue *parent = nullptr;

	void setParent(DictionaryValue *p) { parent = p; }

	void set(const wstring& name, Value *v) { value[name] = v; }

	Value *get(const wstring& name) {
		if (value.count(name)) {
			return value[name];
		} else if (parent) {
			return parent->get(name);
		}
		return nullptr;
	}

	bool has(const wstring& name) {
		return value.count(name) || (parent && parent->has(name));
	}

	string toString() const override;
};

// Indicates that an expression should force exit of func body eval.
class ReturnValue : public Value {
public:
	Value *inner;
	explicit ReturnValue(Value *inner) : Value(ValueType::RETURN), inner(inner) {};
};

class TailCallValue : public Value {
public:
	vector<Value*> args;
	explicit TailCallValue(vector<Value*> _args)
	 : Value(ValueType::TAIL_CALL), args(std::move(_args)) {};
};

class Environment;

enum class FunctionValueType {
	NONE, USER_FUNCTION
};

class FunctionValue : public Value {
public:
	FunctionValue() : Value(ValueType::FUNC) {};

	virtual Value *apply(const vector<Value *> &args, Environment *env, 
		unordered_map<wstring, Value*> *kwargsIn=nullptr) const = 0;

	FunctionValueType functionType = FunctionValueType::NONE;
};

class SyntaxNode;

class UserFunctionValue : public FunctionValue {
	vector<wstring> params;
	bool hasVarKeywordArgs;
	wstring varKeywordArgsParam;
	SyntaxNode *body;
public:
	UserFunctionValue(vector<wstring> params, SyntaxNode *body, 
		Environment *parentEnv)
	 : FunctionValue(), params(std::move(params)), hasVarKeywordArgs(false), body(body),
	   parentEnv(parentEnv) {
		functionType = FunctionValueType::USER_FUNCTION;
	 };

	Environment *parentEnv;

	Value *apply(const vector<Value *> &args, Environment *env,
		unordered_map<wstring, Value*> *kwargsIn=nullptr) const override;

	string toString() const override;

	void setVarKeywordParam(wstring name);
};


#endif