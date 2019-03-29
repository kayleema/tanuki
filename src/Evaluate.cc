#include "Evaluate.h"

Value *Environment::eval(SyntaxNode *tree) {
	if (tree->type == NodeType::CALL) {
		Value *first = eval(tree->children[0]);
		FunctionValue *function = (FunctionValue*)first;
		SyntaxNode *args_tree = tree->children[1]->children[0];
		vector<Value *> args;
		for (auto expression : args_tree->children) {
			args.push_back(eval(expression));
		}
		return function->apply(args);
	}
	if (tree->type == NodeType::TERMINAL) {
		if (tree->content.type == TokenType::SYMBOL) {
			wstring name = tree->content.content;
			if (bindings.count(name)) {
				return bindings[name];
			}
		}
		if (tree->content.type == TokenType::NUMBER) {
			return new NumberValue(tree->content.number);
		}
	}
	return new NumberValue(5);
}

// Global Environment Constructor
Environment::Environment() {
	bindings[L"足す"] = new FunctionSum();
	bindings[L"引く"] = new FunctionDiff();
	bindings[L"表示"] = new FunctionPrint();
}
