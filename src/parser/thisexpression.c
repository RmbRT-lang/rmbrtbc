#include "thisexpression.h"

#include "../assert.h"

void rlc_this_expression_create(
	struct RlcThisExpression * this,
	size_t first)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcThisExpression,
		first);
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
	struct RlcParserData * parser)
{
	int result;
	size_t start = parser->fIndex;
	if(result = rlc_parser_data_consume(
		parser,
		kRlcTokThis))
	{
		rlc_this_expression_create(
			out,
			start);
		RLC_BASE_CAST(out, RlcParsedExpression)->fLast = start;
	}
	return result;
}