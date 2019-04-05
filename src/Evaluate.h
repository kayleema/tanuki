#ifndef EVALUATE_H
#define EVALUATE_H

#include <unordered_map>
#include <unordered_set>
#include "Parser.h"
#include "Value.h"

class Context;

class Environment {
	Value *eval_call(SyntaxNode *node, const FunctionValue* tailContext = nullptr);
	Value *eval_tail(Value* first, SyntaxNode *node, const FunctionValue* tailContext = nullptr);
	Value *eval_calltail(FunctionValue* function, SyntaxNode *node, 
		const FunctionValue* tailContext = nullptr);
	Value *eval_get(DictionaryValue *source, SyntaxNode *tree);
	Value *eval_set(DictionaryValue *source, SyntaxNode *tree);
	Value *eval_terminal(SyntaxNode *node);
	Value *eval_text(SyntaxNode *node, const FunctionValue* tailContext = nullptr);
	Value *eval_function(SyntaxNode *node);
	Value *eval_return(SyntaxNode *node, const FunctionValue* tailContext = nullptr);
	Value *eval_if(SyntaxNode *node);
	Value *eval_assign(SyntaxNode *node);
public:
	Environment(Context *context);
	Environment(Environment *parent, Context *_context=nullptr)
	 : parent(parent) {
	 	if (_context == nullptr) {
	 		if (parent->context == nullptr) {
	 			cout << "error null parent context" << endl;
	 		}
	 		context = parent->context;
	 	} else {
	 		context = _context;
	 	}
	 };

	Environment *parent = nullptr;
	Environment *caller = nullptr;
	Context *context;
	unordered_map<wstring, Value*> bindings;
	Value *lookup(wstring name);
	void bind(wstring name, Value *value);

	Value *eval(SyntaxNode *node, const FunctionValue* tailContext = nullptr);

	Environment *newChildEnvironment();
	void tailReset() {
		bindings.clear();
	}
};

class Context {
	unordered_set<Value *> values;
	unordered_set<Environment *> environments;

	int iteration = 0;
	int frequency = 1;

public:
	unordered_set<Value *> usedValues;
	unordered_set<Environment *> usedEnvironments;

	void mark(Value *value) {
		if (!usedValues.count(value)) {
			usedValues.insert(value);
			if (value->type == ValueType::FUNC &&
				((FunctionValue *)value)->functionType == FunctionValueType::USER_FUNCTION) {
				auto f = (UserFunctionValue *)value;
				mark(f->parentEnv);
			}
			if (value->type == ValueType::DICT) {
				auto d = (DictionaryValue *)value;
				for (auto item : d->value) {
					mark(item.second);
				}
			}
		}
	}

	void mark(Environment *current_env) {
		if (usedEnvironments.count(current_env)) {
			return;
		}
		usedEnvironments.insert(current_env);
		if (current_env->parent) {
			mark(current_env->parent);
		}
		if (current_env->caller) {
			mark(current_env->caller);
		}
		for (auto binding : current_env->bindings) {
			mark(binding.second);
		}
	}

	void collect(Environment *current_env) {
		iteration++;
		if ((iteration % frequency != 0)) {
			return;
		}

		usedValues.clear();
		usedEnvironments.clear();
		mark(current_env);
		for (auto *value : values) {
			if (!usedValues.count(value)) {
				if (value->refs){
					usedValues.insert(value);
				} else {
					delete value;
				}
			} else {
			}
		}
		values = usedValues;
		for (auto *environment : environments) {
			if (!usedEnvironments.count(environment)) {
				delete environment;
			}
		}
		environments = usedEnvironments;
	}

	void cleanup() {
		for (auto value : values) {
			delete value;
		}
		for (auto environment : environments) {
			delete environment;
		}
	}

	void setFrequency(int freq) {
		frequency = freq;
	}

	Value *newNoneValue() {
		static Value staticNone(ValueType::NONE);
		return &staticNone;
	}

	NumberValue *newNumberValue(int number) {
		auto result = new NumberValue(number);
		values.insert(result);
		return result;
	}

	StringValue *newStringValue(wstring str) {
		auto result = new StringValue(str);
		values.insert(result);
		return result;
	}

	DictionaryValue *newDictionaryValue() {
		auto result = new DictionaryValue();
		values.insert(result);
		return result;
	}

	UserFunctionValue *newUserFunctionValue(
			vector<wstring> params, SyntaxNode *body, Environment *e) {
		auto result = new UserFunctionValue(params, body, e);
		values.insert(result);
		return result;
	}

	void trackValue(Value *value) {
		values.insert(value);
	}

	Environment *newChildEnvironment(Environment *e) {
		auto result = new Environment(e);
		environments.insert(result);
		return result;
	}
};	

#endif