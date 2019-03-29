#ifndef EVALUATE_H
#define EVALUATE_H

#include <unordered_map>
#include "Parser.h"
#include "Value.h"

class Environment {
	Value *eval_call(SyntaxNode *node);
	Value *eval_terminal(SyntaxNode *node);
	Value *eval_text(SyntaxNode *node);
public:
	Environment();

	unordered_map<wstring, Value*> bindings;

	Value *eval(SyntaxNode *node);
};

#endif