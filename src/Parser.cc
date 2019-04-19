#include <iostream>
#include "Parser.h"

SyntaxNode *Parser::run() {
	allTokens = lexer->getAllTokens();
	currentTokenIndex = 0;
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
	if ((result = run_assign())) {
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
		while (accept(TokenType::ELIF)) {
			expect(TokenType::COMMA);
			SyntaxNode *conditionElif = run_expression();
			expect(TokenType::NEWL);
			expect(TokenType::INDENT);
			SyntaxNode *bodyElif = run_text();
			expect(TokenType::DEDENT);
			result->children.push_back(conditionElif);
			result->children.push_back(bodyElif);
		}
		if (accept(TokenType::ELSE)) {
			expect(TokenType::NEWL);
			expect(TokenType::INDENT);
			SyntaxNode *bodyElse = run_text();
			result->children.push_back(bodyElse);
			expect(TokenType::DEDENT);
		}
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

SyntaxNode *Parser::run_assign() {
	Token lhs;
	if (accept({TokenType::SYMBOL, TokenType::ASSIGN}, {&lhs, nullptr})) {
		auto rhs = run_expression();
		auto result = new SyntaxNode(NodeType::ASSIGN);
		result->children.push_back(new SyntaxNode(lhs));
		result->children.push_back(rhs);
		return result;
	}
	return nullptr;
}

SyntaxNode *Parser::run_function() {
	if(!accept(TokenType::FUNC)) {
		return nullptr;
	}
	if(!expect(TokenType::COMMA)) {
		return nullptr;
	}
	Token name;
	accept(TokenType::SYMBOL, &name);
	auto result = new SyntaxNode(NodeType::FUNC);
	result->children.push_back(new SyntaxNode(name));
	if(!expect(TokenType::LPAREN)) {
		return nullptr;
	}
	auto params = new SyntaxNode(NodeType::PARAMS);
	result->children.push_back(params);
	while (!accept(TokenType::RPAREN)){
		Token param;
		if(accept(TokenType::SYMBOL, &param)){
			params->children.push_back(new SyntaxNode(param));
			accept(TokenType::COMMA);
		}
	}
	if(!expect(TokenType::NEWL) || !expect(TokenType::INDENT)){
		return nullptr;
	}
	result->children.push_back(run_text());
	if(!expect(TokenType::DEDENT)) {
		return nullptr;
	}
	return result;
}

SyntaxNode *Parser::run_expression() {
	Token start;
	if (accept(TokenType::STRING, &start)) {
		return new SyntaxNode(start);
	}
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
	if (accept(TokenType::MINUS, &start)) {
		if (accept(TokenType::NUMBER, &start)) {
			start.number = -start.number;
			return new SyntaxNode(start);
		}
		return nullptr;
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
	Token symbol;
	if (accept({TokenType::DOT, TokenType::SYMBOL}, {nullptr, &symbol}, {TokenType::ASSIGN})) {
		auto node = new SyntaxNode(NodeType::GET);
		node->children.push_back(new SyntaxNode(symbol));
		auto tail = run_expression_tail();
		if (tail) {
			node->children.push_back(tail);
		}
		return node;
	}
	if (accept(
			{TokenType::DOT, TokenType::SYMBOL, TokenType::ASSIGN}, 
			{nullptr, &symbol, nullptr})) {
		auto node = new SyntaxNode(NodeType::SET);
		node->children.push_back(new SyntaxNode(symbol));
		auto rhs = run_expression();
		if (rhs) {
			node->children.push_back(rhs);
		}
		return node;
	}
	return nullptr;
}

SyntaxNode *Parser::run_args() {
	auto node = new SyntaxNode(NodeType::ARGS);
	if (currentToken().type == TokenType::RPAREN) {
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
	if (currentToken().type == type) {
		if (out) {
			*out = currentToken();
		}
		currentTokenIndex++;
		return true;
	}
	return false;
}

bool Parser::accept(vector<TokenType> types, vector<Token *> outs) {
	int i = 0;
	for (auto type : types) {
		size_t tokenIndex = currentTokenIndex + i;
		if (tokenIndex >= allTokens.size()) {
			break;
		}
		if(allTokens[tokenIndex].type != type) {
			return false;
		}
		i++;
	}
	i = 0;
	for (auto out : outs) {
		if(out) {
			*out = allTokens[currentTokenIndex + i];
		}
		i++;
	}

	currentTokenIndex += types.size();
	return true;
}

bool Parser::accept(vector<TokenType> types, vector<Token *> outs, vector<TokenType> rejectTypes) {
	int i = 0;
	for (auto rejectType : rejectTypes) {
		size_t tokenIndex = currentTokenIndex + types.size() + i;
		if (tokenIndex >= allTokens.size()) {
			break;
		}
		if (allTokens[tokenIndex].type == rejectType) {
			return false;
		}
		i++;
	}
	return Parser::accept(types, outs);
}

bool Parser::expect(TokenType type) {
	if (accept(type)) {
		return true;
	}
	cout << "unexpected " << currentToken().toString() << endl;
	cout << "  expected " << Token(type, L"", -1).toString() << endl;
	return false;
}

Token Parser::currentToken() {
	return allTokens[currentTokenIndex];
}

string SyntaxNode::toString(int indent) {
	ostringstream result("");

	string indentation(indent, ' ');
	result << indentation;

	result << NodeTypeStrings[(int)type];
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
