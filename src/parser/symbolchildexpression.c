#include "symbolchildexpression.h"
#include "../assert.h"


void rlc_parsed_symbol_child_expression_create(
	struct RlcParsedSymbolChildExpression * this,
	RlcSrcIndex first)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedSymbolChildExpression,
		first,
		rlc_src_string_end(&RLC_BASE_CAST(this, RlcParsedSymbolChild)->fName));
}

void rlc_parsed_symbol_child_expression_destroy(
	struct RlcParsedSymbolChildExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));

	rlc_parsed_symbol_child_destroy(
		RLC_BASE_CAST(this, RlcParsedSymbolChild));
}

int rlc_parsed_symbol_child_expression_parse(
	struct RlcParsedSymbolChildExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	RlcSrcIndex const start = rlc_parser_index(parser);

	if(!rlc_parsed_symbol_child_parse(
		RLC_BASE_CAST(out, RlcParsedSymbolChild),
		parser))
	{
		return 0;
	}

	rlc_parsed_symbol_child_expression_create(
		out,
		start);

	return 1;
}