#include "Evaluate.h"

Value *Environment::eval(SyntaxNode *tree) {
	context->collect(this);
	if (tree->type == NodeType::CALL) {
		return eval_call(tree);
	}
	if (tree->type == NodeType::TERMINAL) {
		return eval_terminal(tree);
	}
	if (tree->type == NodeType::TEXT) {
		return eval_text(tree);
	}
	if (tree->type == NodeType::FUNC) {
		return eval_function(tree);
	}
	if (tree->type == NodeType::RETURN) {
		return eval_return(tree);
	}
	if (tree->type == NodeType::IF) {
		return eval_if(tree);
	}
	return context->newNoneValue();
}

Value *Environment::eval_call(SyntaxNode *tree) {
	Value *first = eval(tree->children[0]);
	first->refs++;
	FunctionValue *function = (FunctionValue*)first;
	SyntaxNode *tail = tree->children[1];

	// auto result = eval_calltail(function, tail);

	SyntaxNode *args_tree = tail->children[0];
	vector<Value *> args;
	for (auto expression : args_tree->children) {
		auto arg = eval(expression);
		arg->refs++;
		args.push_back(arg);
	}
	auto result = function->apply(args, this);
	result->refs++;

	if (tree->children[1]->children.size() == 2) {
		cout << "high order" << endl;

	}

	for (auto arg : args) {
		arg->refs--;
	}
	result->refs--;
	first->refs--;
	return result;
}

Value *Environment::eval_calltail(SyntaxNode *tree) {

}

Value *Environment::eval_terminal(SyntaxNode *tree) {
	if (tree->content.type == TokenType::SYMBOL) {
		wstring name = tree->content.content;
		return lookup(name);
	}
	if (tree->content.type == TokenType::NUMBER) {
		auto result = context->newNumberValue(tree->content.number);
		return result;
	}
	return context->newNoneValue();
}

Value *Environment::eval_text(SyntaxNode *tree) {
	for (auto statement : tree->children) {
		Value *statementReturnValue = eval(statement);
		if (statementReturnValue->type == ValueType::RETURN) {
			return statementReturnValue;
		} else if (statementReturnValue != nullptr && 
				statementReturnValue->type != ValueType::NONE) {
			// delete statementReturnValue;
		}
	}
	return context->newNoneValue();
}

Value *Environment::eval_function(SyntaxNode *tree) {
	std::vector<wstring> params;
	wstring name = tree->children[0]->content.content;
	for (auto param : tree->children[1]->children) {
		params.push_back(param->content.content);
	}
	auto body = tree->children[2];
	auto function = context->newUserFunctionValue(params, body, this);
	bindings[name] = function;
	return context->newNoneValue();
}

Value *Environment::eval_return(SyntaxNode *tree) {
	return new ReturnValue(eval(tree->children[0]));
}

Value *Environment::eval_if(SyntaxNode *tree) {
	SyntaxNode *condition = tree->children[0];
	SyntaxNode *body = tree->children[1];
	Value *condValue = eval(condition);
	if (condValue->isTruthy()) {
		auto result = eval(body);
		return result;
	}
	return context->newNoneValue();
}

Value *Environment::lookup(wstring name) {
	if (bindings.count(name)) {
		return bindings[name];
	}
	if (parent) {
		return parent->lookup(name);
	}
	return nullptr;
}

void Environment::bind(wstring name, Value *value) {
	bindings[name] = value;
}

Environment *Environment::newChildEnvironment() {
	return context->newChildEnvironment(this);
}

// Global Environment Constructor
Environment::Environment(Context *context): context(context) {
	bindings[L"足す"] = new FunctionSum();
	bindings[L"引く"] = new FunctionDiff();
	bindings[L"表示"] = new FunctionPrint();
	bindings[L"イコール"] = new FunctionEqual();
}
