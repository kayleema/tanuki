#include <iostream>
#include "Parser.h"

SyntaxNode *Parser::run() {
	current = lexer->getToken();
	return run_expression();
}

SyntaxNode *Parser::run_expression() {
	Token start;
	if (accept(TokenType::SYMBOL, &start)) {
		auto tail = run_expression_tail();
		if (tail) {
			auto result = new SyntaxNode(NodeType::CALL);
			result->children.push_back(new SyntaxNode(start));
			result->children.push_back(tail);
			return result;
		} else {
			return new SyntaxNode(start);
		}
	}
	if (accept(TokenType::NUMBER, &start)) {
		return new SyntaxNode(start);
	}
	return nullptr;
}

SyntaxNode *Parser::run_expression_tail() {
	if (accept(TokenType::LPAREN)) {
		auto node = new SyntaxNode(NodeType::CALL_TAIL);
		auto args = run_args();
		if(!accept(TokenType::RPAREN)) {
			cout << "error";
		}
		auto tail = run_expression_tail();
		node->children.push_back(args);
		if (tail) {
			node->children.push_back(tail);
		}
		return node;
	}
	return nullptr;
}

SyntaxNode *Parser::run_args() {
	auto node = new SyntaxNode(NodeType::ARGS);
	if (current.type == TokenType::RPAREN) {
		return node;
	} else {
		do {
			SyntaxNode *arg = run_expression();
			node->children.push_back(arg);
		} while (accept(TokenType::COMMA));
	}
	return node;
}

bool Parser::accept(TokenType type, Token *out) {
	if (current.type == type) {
		// cout << "accept " << current.toString() << endl;
		if (out) {
			*out = current;
		}
		current = lexer->getToken();
		return true;
	}
	// cout << "not accept " << current.toString() << endl;
	return false;
}

bool Parser::expect(TokenType type) {
	if (accept(type)) {
		return true;
	}
	cout << "unexpected" << endl;
	return false;
}

string SyntaxNode::toString(int indent) {
	ostringstream result("");

	string indentation(indent, ' ');
	result << indentation;

	const string ss[] = {"CALL", "TERMINAL", "ARGS", "CALL_TAIL"};
	result << ss[(int)type];
	if (type == NodeType::TERMINAL) {
		result << " " << content.toString();
	}
	result << endl;
	for (auto child : children) {
		result << child->toString(indent + 1);
	}
	return result.str();
}
