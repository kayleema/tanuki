#include <utility>

#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

class Environment;

using namespace std;

enum class ValueType {
    NUM, NUM_FLOAT, FUNC, NONE, RETURN, STRING, TAIL_CALL, DICT, MODULE, ARRAY
};
const string ValueTypeStrings[] = {
        "NUM", "NUM_FLOAT", "FUNC", "NONE", "RETURN", "STRING", "TAIL_CALL", "DICT", "MODULE", "ARRAY"
};

class NumberValue;
class FloatValue;
class StringValue;
class DictionaryValue;
class FunctionValue;

class Value {
public:
    explicit Value(ValueType t) : type(t) {
    };

    virtual ~Value() = default;

    virtual bool equals(const Value *rhs) const;

    virtual string toString() const;

    virtual string toStringJP() const;

    NumberValue *toNumberValue() { return (NumberValue *) this; }

    FunctionValue *toFunctionValue() { return (FunctionValue *) this; }

    FloatValue *toFloatValue() { return (FloatValue *) this; }

    StringValue *toStringValue() { return (StringValue *) this; }

    DictionaryValue *toDictionaryValue() { return (DictionaryValue *) this; }

    virtual bool isTruthy() const { return true; };

    virtual DictionaryValue *getLookupSource(Environment *env);

    ValueType type;
    int refs = 0;
};

bool operator==(const Value &lhs, const Value &rhs);

class NumberValue : public Value {
public:
    explicit NumberValue(long n) : Value(ValueType::NUM), value(n) {};

    bool equals(const Value *rhs) const override;

    string toString() const override;
    string toStringJP() const override;

    bool isTruthy() const override { return value != 0; };

    DictionaryValue *getLookupSource(Environment *env) override;

    long value;
};

class FloatValue : public Value {
public:
    explicit FloatValue(double n) : Value(ValueType::NUM_FLOAT), value(n) {};

    bool equals(const Value *rhs) const override;

    string toString() const override;
    string toStringJP() const override;

    bool isTruthy() const override { return value != 0; };

    DictionaryValue *getLookupSource(Environment *env) override;

    double value;
};

class StringValue : public Value {
public:
    explicit StringValue(wstring str) : Value(ValueType::STRING), value(std::move(str)) {};

    bool equals(const Value *rhs) const override;

    string toString() const override;
    string toStringJP() const override;

    DictionaryValue *getLookupSource(Environment *env) override;

    wstring value;
};

class DictionaryValue : public Value {

public:
    DictionaryValue();

    unordered_map<wstring, Value *> value;
    DictionaryValue *parent = nullptr;

    void setParent(DictionaryValue *p) { parent = p; }

    DictionaryValue *getLookupSource(Environment *env) override;

    void set(const wstring &name, Value *v) { value[name] = v; }

    virtual Value *get(const wstring &name) {
        if (value.count(name)) {
            return value[name];
        } else if (parent) {
            return parent->get(name);
        }
        return nullptr;
    }

    virtual bool has(const wstring &name) {
        return value.count(name) || (parent && parent->has(name));
    }

    bool equals(const Value *rhs) const override;

    string toString() const override;
    string toStringJP() const override;
};


class ArrayValue : public DictionaryValue {
public:
    vector<Value *> value;

    explicit ArrayValue() {
        type = ValueType::ARRAY;
        parent = nullptr;
    }

    void set(long index, Value *v) { value[index] = v; }

    bool has(const wstring &name) override {
        return (parent && parent->has(name));
    }

    void push(Value *v) { value.push_back(v); }

    Value *getIndex(long index) {
        if ((size_t) index < value.size()) {
            return value[index];
        }
        return nullptr;
    }

    long length() const {
        return value.size();
    }

    string toString() const override;
    string toStringJP() const override;
};

// Indicates that an expression should force exit of func body eval.
class ReturnValue : public Value {
public:
    Value *inner;

    explicit ReturnValue(Value *inner) : Value(ValueType::RETURN), inner(inner) {};
};

class TailCallValue : public Value {
public:
    vector<Value *> args;
    bool hasKwArgs = false;
    unordered_map<wstring, Value *> kwArgs;

    explicit TailCallValue(vector<Value *> _args)
            : Value(ValueType::TAIL_CALL), args(std::move(_args)) {};
};

enum class FunctionValueType {
    NONE, USER_FUNCTION, BOUND_FUNCTION
};

class FunctionValue : public Value {
public:
    FunctionValue() : Value(ValueType::FUNC) {};

    virtual Value *apply(const vector<Value *> &args, Environment *env,
                         unordered_map<wstring, Value *> *kwargsIn = nullptr) const = 0;

    FunctionValueType functionType = FunctionValueType::NONE;

    DictionaryValue *getLookupSource(Environment *env) override;

    string toStringJP() const override;
};

class SyntaxNode;

class UserFunctionValue : public FunctionValue {
    vector<wstring> params;
    unordered_map<wstring, Value *> paramsWithDefault;
    bool hasVarKeywordArgs;
    wstring varKeywordArgsParam;
    bool hasVarArgs{};
    wstring varArgsParam;
    SyntaxNode *body;
public:
    UserFunctionValue(vector<wstring> params, SyntaxNode *body,
                      Environment *parentEnv)
            : FunctionValue(), params(std::move(params)), hasVarKeywordArgs(false), body(body),
              parentEnv(parentEnv) {
        functionType = FunctionValueType::USER_FUNCTION;
    };

    UserFunctionValue(vector<wstring> params, unordered_map<wstring, Value *> paramsWithDefault,
                      SyntaxNode *body, Environment *parentEnv)
            : FunctionValue(), params(std::move(params)), paramsWithDefault(std::move(paramsWithDefault)),
              hasVarKeywordArgs(false), body(body), parentEnv(parentEnv) {
        functionType = FunctionValueType::USER_FUNCTION;
    };

    Environment *parentEnv;

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *kwargsIn = nullptr) const override;

    string toString() const override;

    const unordered_map<wstring, Value *> &getParamsWithDefault() {
        return paramsWithDefault;
    }

    void setVarKeywordParam(wstring name);

    bool hasVarKeywordParam() const { return hasVarKeywordArgs; };

    const wstring getVarKeywordParam() const { return varKeywordArgsParam; };

    void setVarParam(wstring name);

    bool hasVarParam() const { return hasVarArgs; };

    const wstring getVarParam() const { return varArgsParam; };
};

class BoundFunctionValue : public FunctionValue {
public:
    FunctionValue *function;
    Value *jibun;

    BoundFunctionValue(FunctionValue *function, Value *jibun)
            : FunctionValue(), function(function), jibun(jibun) {
        functionType = FunctionValueType::BOUND_FUNCTION;
    };

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *kwargsIn = nullptr) const override;
};

#endif