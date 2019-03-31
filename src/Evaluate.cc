#include "Evaluate.h"

Value *Environment::eval(SyntaxNode *tree, 
		const FunctionValue* tailContext) {
	context->collect(this);
	if (tree->type == NodeType::CALL) {
		return eval_call(tree, tailContext);
	}
	if (tree->type == NodeType::TERMINAL) {
		return eval_terminal(tree);
	}
	if (tree->type == NodeType::TEXT) {
		return eval_text(tree, tailContext);
	}
	if (tree->type == NodeType::FUNC) {
		return eval_function(tree);
	}
	if (tree->type == NodeType::RETURN) {
		return eval_return(tree, tailContext);
	}
	if (tree->type == NodeType::IF) {
		return eval_if(tree);
	}
	if (tree->type == NodeType::ASSIGN) {
		return eval_assign(tree);
	}
	return context->newNoneValue();
}

Value *Environment::eval_call(SyntaxNode *tree, 
		const FunctionValue* tailContext) {
	Value *first = eval(tree->children[0]);
	first->refs++;
	FunctionValue *function = (FunctionValue*)first;
	SyntaxNode *tail = tree->children[1];

	auto result = eval_calltail(function, tail, tailContext);

	first->refs--;
	return result;
}

Value *Environment::eval_calltail(FunctionValue *function, SyntaxNode *tail, 
		const FunctionValue* tailContext) {
	if (tail->children.size() == 2) {
		tailContext = nullptr;
	}

	SyntaxNode *args_tree = tail->children[0];
	vector<Value *> args;
	for (auto expression : args_tree->children) {
		auto arg = eval(expression);
		arg->refs++;
		args.push_back(arg);
	}
	if (function == tailContext) {
		for (auto arg : args) {
			arg->refs--;
		}
		return new TailCallValue(args);
	}
	auto result = function->apply(args, this);
	auto finalResult = result;
	result->refs++;

	if (tail->children.size() == 2) {
		auto nextTail = tail->children[1];
		finalResult = eval_calltail((FunctionValue*)result, nextTail);
	}

	for (auto arg : args) {
		arg->refs--;
	}
	result->refs--;
	return finalResult;
}

Value *Environment::eval_terminal(SyntaxNode *tree) {
	if (tree->content.type == TokenType::SYMBOL) {
		wstring name = tree->content.content;
		auto result = lookup(name);
		if (!result) {
			return context->newNoneValue();
		}
		return lookup(name);
	}
	if (tree->content.type == TokenType::NUMBER) {
		auto result = context->newNumberValue(tree->content.number);
		return result;
	}
	if (tree->content.type == TokenType::STRING) {
		auto result = context->newStringValue(tree->content.content);
		return result;
	}
	return context->newNoneValue();
}

Value *Environment::eval_text(SyntaxNode *tree, 
		const FunctionValue* tailContext) {
	for (auto statement : tree->children) {
		Value *statementReturnValue = eval(statement, tailContext);
		if (statementReturnValue->type == ValueType::RETURN ||
			statementReturnValue->type == ValueType::TAIL_CALL) {
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

Value *Environment::eval_return(SyntaxNode *tree, 
		const FunctionValue* tailContext) {
	auto result = eval(tree->children[0], tailContext);
	if (result->type == ValueType::TAIL_CALL) {
		return result;
	}
	return new ReturnValue(result);
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

Value *Environment::eval_assign(SyntaxNode *tree) {
	wstring lhs = tree->children[0]->content.content;
	bind(lhs, eval(tree->children[1]));
	return context->newNoneValue();
}

Value *Environment::lookup(wstring name) {
	if (bindings.count(name)) {
		return bindings[name];
	}
	if (parent) {
		return parent->lookup(name);
	}
	cout << "lookup failure" << endl;
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
