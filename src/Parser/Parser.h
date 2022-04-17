#ifndef PARSER_H
#define PARSER_H

#include "GenericParser.h"

class Parser : public GenericParser {
public:
    using GenericParser::GenericParser;

    SyntaxNode *run_text() override;

    SyntaxNode *run_statement();

    SyntaxNode *run_return();

    SyntaxNode *run_import();

    SyntaxNode *run_nonlocal();

    SyntaxNode *run_if();

    SyntaxNode *run_assert();

    SyntaxNode *run_assign();

    SyntaxNode *run_function();

    SyntaxNode *run_function_param();

    SyntaxNode *run_expression();

    SyntaxNode *run_infix_expression();

    SyntaxNode *run_infix_comparison_expression();

    SyntaxNode *run_infix_additive_expression();

    SyntaxNode *run_infix_multiplicative_expression();

    SyntaxNode *run_args();

    SyntaxNode *run_expression_tail();

    SyntaxNode *run_passed_lambda();
};

#endif
