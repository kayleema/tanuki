#ifndef EVALUATE_H
#define EVALUATE_H

#include <unordered_map>
#include "Parser.h"
#include "Value.h"

class Environment {
	Environment *parent = nullptr;

	Value *eval_call(SyntaxNode *node);
	Value *eval_terminal(SyntaxNode *node);
	Value *eval_text(SyntaxNode *node);
	Value *eval_function(SyntaxNode *node);
	Value *eval_return(SyntaxNode *node);
	Value *eval_if(SyntaxNode *node);
public:
	Environment();
	Environment(Environment *parent) : parent(parent) {};

	unordered_map<wstring, Value*> bindings;
	Value *lookup(wstring name);
	void bind(wstring name, Value *value);

	Value *eval(SyntaxNode *node);
};

#endif