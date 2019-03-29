#include "Evaluate.h"

Value *Environment::eval(SyntaxNode *tree) {
	if (tree->type == NodeType::CALL) {
		return eval_call(tree);
	}
	if (tree->type == NodeType::TERMINAL) {
		return eval_terminal(tree);
	}
	if (tree->type == NodeType::TEXT) {
		return eval_text(tree);
	}
	return new NumberValue(44444444);
}

Value *Environment::eval_call(SyntaxNode *tree) {
	Value *first = eval(tree->children[0]);
	FunctionValue *function = (FunctionValue*)first;
	SyntaxNode *args_tree = tree->children[1]->children[0];
	vector<Value *> args;
	for (auto expression : args_tree->children) {
		args.push_back(eval(expression));
	}
	return function->apply(args);
}

Value *Environment::eval_terminal(SyntaxNode *tree) {
	if (tree->content.type == TokenType::SYMBOL) {
		wstring name = tree->content.content;
		if (bindings.count(name)) {
			return bindings[name];
		}
	}
	if (tree->content.type == TokenType::NUMBER) {
		return new NumberValue(tree->content.number);
	}
	return new Value(ValueType::NONE);
}

Value *Environment::eval_text(SyntaxNode *tree) {
	for (auto statement : tree->children) {
		eval(statement);
	}
	return new Value(ValueType::NONE);
}

// Global Environment Constructor
Environment::Environment() {
	bindings[L"足す"] = new FunctionSum();
	bindings[L"引く"] = new FunctionDiff();
	bindings[L"表示"] = new FunctionPrint();
}
