#include "expressionstatement.h"
#include "operatorexpression.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_expression_statement_create(
	struct RlcParsedExpressionStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedExpressionStatement);

	this->fExpression = NULL;
}

void rlc_parsed_expression_statement_destroy(
	struct RlcParsedExpressionStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fExpression);
		rlc_free((void**)&this->fExpression);
	}
	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_expression_statement_parse(
	struct RlcParsedExpressionStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_expression_statement_create(
		out);

	out->fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType));


	if(!out->fExpression)
	{
		rlc_parsed_expression_statement_destroy(out);
		return 0;
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	return 1;
}

void rlc_parsed_expression_statement_print(
	struct RlcParsedExpressionStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	// Treat top-level variadic expansion expressions as folding expressions.
	struct RlcParsedOperatorExpression * e;
	if((e = RLC_DYNAMIC_CAST(
		this->fExpression,
		RlcParsedExpression,
		RlcParsedOperatorExpression))
	&& e->fOperator == kVariadicExpand)
	{
		RLC_DASSERT(e->fExpressionCount == 1);
		fputs("(", out);
		rlc_parsed_expression_print(e->fExpressions[0], file, out);
		fputs(", ...)", out);
	} else
		rlc_parsed_expression_print(this->fExpression, file, out);
	fputs(";\n", out);
}