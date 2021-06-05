#include "returnstatement.h"
#include "../assert.h"
#include "../malloc.h"
#include "../tokeniser/tokens.h"

void rlc_parsed_return_statement_create(
	struct RlcParsedReturnStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedReturnStatement);
	this->fExpression = NULL;
}

void rlc_parsed_return_statement_destroy(
	struct RlcParsedReturnStatement * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this,RlcParsedStatement)) == kRlcParsedReturnStatement);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));

	if(this->fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fExpression);
		rlc_free((void**)&this->fExpression);
	}
}

static _Thread_local int forbidden = 0;

int rlc_parsed_return_statement_parse(
	struct RlcParsedReturnStatement * out,
	struct RlcParser * parser)
{
	if(forbidden && rlc_parser_is_current(parser, kRlcTokReturn))
		rlc_parser_fail(parser, "forbidden in this context");

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokReturn))
		return 0;

	rlc_parsed_return_statement_create(out);
	out->fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	return 1;
}

void rlc_parsed_return_statement_print(
	struct RlcParsedReturnStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fputs("_return ", out);
	if(this->fExpression)
		rlc_parsed_expression_print(this->fExpression, file, out);
	fputs(";\n", out);
}


void rlc_parsed_return_statement_forbid()
{
	forbidden++;
}
void rlc_parsed_return_statement_allow()
{
	forbidden--;
}
