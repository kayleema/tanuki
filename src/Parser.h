#ifndef PARSER_H
#define PARSER_H
#include "Tokenizer.h"

class SyntaxNode;

class Parser {
	Tokenizer *lexer;
	Token current;
	vector<Token> allTokens;
public:
	Parser(Tokenizer *t) : lexer(t) {}

	SyntaxNode *run();
	SyntaxNode *run_text();
	SyntaxNode *run_statement();
	SyntaxNode *run_return();
	SyntaxNode *run_if();
	SyntaxNode *run_function();
	SyntaxNode *run_expression();
	SyntaxNode *run_args();
	SyntaxNode *run_expression_tail();
	
	bool accept(TokenType type, Token *out=nullptr);
	bool expect(TokenType type);
};

enum class NodeType {
	CALL, TERMINAL, ARGS, CALL_TAIL, TEXT, FUNC, PARAMS, RETURN, IF
};

class SyntaxNode {
public:
	NodeType type;
	Token content;
	vector<SyntaxNode*> children;

	SyntaxNode(NodeType _type) : type(_type) {}
	SyntaxNode(Token _content) : type(NodeType::TERMINAL), content(_content) {}

	~SyntaxNode();

	string toString(int indent = 0);
};

#endif