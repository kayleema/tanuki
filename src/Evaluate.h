#ifndef EVALUATE_H
#define EVALUATE_H

#include <unordered_map>
#include "Parser.h"
#include "Value.h"

class Environment {
public:
	Environment();

	unordered_map<wstring, Value*> bindings;

	Value *eval(SyntaxNode *node);
};

#endif