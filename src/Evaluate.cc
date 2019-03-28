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

bool Value::equals(Value *rhs) const{
	return (type == rhs->type);
}

bool NumberValue::equals(Value *rhs) const{
	return Value::equals(rhs) && (value == ((NumberValue *)rhs)->value);
}

string Value::toString() const{
	return "generic value";
}
string NumberValue::toString() const{
	ostringstream result;
	result << "NumberValue(" << value << ")";
	return result.str();
}

