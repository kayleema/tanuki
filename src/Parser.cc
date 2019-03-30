#include <iostream>
#include "Parser.h"

SyntaxNode *Parser::run() {
	current = lexer->getToken();
	return run_text();
}

SyntaxNode *Parser::run_text() {
	auto result = new SyntaxNode(NodeType::TEXT);

	do {
		while (accept(TokenType::NEWL)) {}
		SyntaxNode *statement = run_statement();
		if (!statement) {
			return result;
		}
		result->children.push_back(statement);
	} while(true);
}

SyntaxNode *Parser::run_statement() {
	SyntaxNode *result;
	if ((result = run_return())) {
		return result;
	}
	if ((result = run_if())) {
		return result;
	}
	if ((result = run_function())) {
		return result;
	}
	return run_expression();
}

SyntaxNode *Parser::run_if() {
	if(accept(TokenType::IF)) {
		expect(TokenType::COMMA);
		SyntaxNode *condition = run_expression();
		expect(TokenType::NEWL);
		expect(TokenType::INDENT);
		SyntaxNode *body = run_text();
		expect(TokenType::DEDENT);
		auto result = new SyntaxNode(NodeType::IF);
		result->children.push_back(condition);
		result->children.push_back(body);
		return result;
	}
	return nullptr;
}

SyntaxNode *Parser::run_return() {
	if (accept(TokenType::RETURN)) {
		expect(TokenType::COMMA);
		SyntaxNode *rhs = run_expression();
		auto result = new SyntaxNode(NodeType::RETURN);
		result->children.push_back(rhs);
		return result;
	}
	return nullptr;
}

SyntaxNode *Parser::run_function() {
	if(!accept(TokenType::FUNC)) {
		return nullptr;
	}
	expect(TokenType::COMMA);
	Token name;
	accept(TokenType::SYMBOL, &name);
	auto result = new SyntaxNode(NodeType::FUNC);
	result->children.push_back(new SyntaxNode(name));
	expect(TokenType::LPAREN);
	auto params = new SyntaxNode(NodeType::PARAMS);
	result->children.push_back(params);
	while (!accept(TokenType::RPAREN)){
		Token param;
		if(accept(TokenType::SYMBOL, &param)){
			params->children.push_back(new SyntaxNode(param));
			accept(TokenType::COMMA);
		}
	}
	expect(TokenType::NEWL);
	expect(TokenType::INDENT);
	result->children.push_back(run_text());
	expect(TokenType::DEDENT);
	return result;
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
	cout << "unexpected " << current.toString() << endl;
	cout << "           " << Token(type, L"", -1).toString() << endl;
	return false;
}

string SyntaxNode::toString(int indent) {
	ostringstream result("");

	string indentation(indent, ' ');
	result << indentation;

	const string ss[] = {
		"CALL", "TERMINAL", "ARGS", "CALL_TAIL", "TEXT", "FUNC", "PARAMS",
		"RETURN", "IF"
	};
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

SyntaxNode::~SyntaxNode() {
	for (auto child : children) {
		delete child;
	}
}
