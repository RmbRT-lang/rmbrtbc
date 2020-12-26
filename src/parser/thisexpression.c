#include "thisexpression.h"

#include "../assert.h"

void rlc_parsed_this_expression_create(
	struct RlcParsedThisExpression * this,
	struct RlcToken token)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedThisExpression,
		token,
		token);
}

void rlc_parsed_this_expression_destroy(
	struct RlcParsedThisExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_this_expression_parse(
	struct RlcParsedThisExpression * out,
	struct RlcParser * parser)
{
	int result;
	struct RlcToken token;
	if((result = rlc_parser_consume(
		parser,
		&token,
		kRlcTokThis)))
	{
		rlc_parsed_this_expression_create(
			out,
			token);
	}
	return result;
}

void rlc_parsed_this_expression_print(
	struct RlcParsedThisExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	(void) this;
	(void) file;
	fprintf(out, "(*this)");
}