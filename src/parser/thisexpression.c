#include "thisexpression.h"

#include "../assert.h"

void rlc_this_expression_create(
	struct RlcThisExpression * this,
	struct RlcSrcString const * string)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcThisExpression,
		string->start,
		rlc_src_string_end(string));
}

void rlc_this_expression_destroy(
	struct RlcThisExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_this_expression_parse(
	struct RlcThisExpression * out,
	struct RlcParser * parser)
{
	int result;
	struct RlcToken token;
	if((result = rlc_parser_consume(
		parser,
		&token,
		kRlcTokThis)))
	{
		rlc_this_expression_create(
			out,
			&token.content);
	}
	return result;
}