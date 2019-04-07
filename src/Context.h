#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_set>

#include "Evaluate.h"

class Context {
	unordered_set<Value *> values;
	unordered_set<Environment *> environments;

	int iteration = 0;
	int frequency = 1;

public:
	unordered_set<Value *> usedValues;
	unordered_set<Environment *> usedEnvironments;

	void mark(Value *value);

	void mark(Environment *current_env);

	void collect(Environment *current_env);

	void cleanup();

	void setFrequency(int freq);

	Value *newNoneValue();
	NumberValue *newNumberValue(int number);
	StringValue *newStringValue(wstring str);
	DictionaryValue *newDictionaryValue();
	UserFunctionValue *newUserFunctionValue(
			vector<wstring> params, SyntaxNode *body, Environment *e);

	void trackValue(Value *value);

	Environment *newChildEnvironment(Environment *e);
};

#endif
