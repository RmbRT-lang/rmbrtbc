#include "symbolexpression.h"
#include "../assert.h"

void rlc_parsed_symbol_expression_create(
	struct RlcParsedSymbolExpression * this,
	size_t first)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedSymbolExpression,
		first);

	rlc_parsed_symbol_create(&this->fSymbol);
}

void rlc_parsed_symbol_expression_destroy(
	struct RlcParsedSymbolExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this, RlcParsedExpression)) == kRlcParsedSymbolExpression);

	rlc_parsed_symbol_destroy(&this->fSymbol);
}

int rlc_parsed_symbol_expression_parse(
	struct RlcParsedSymbolExpression * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_symbol_expression_create(
		out,
		parser->fIndex);

	if(!rlc_parsed_symbol_parse(
		&out->fSymbol,
		parser))
	{
		rlc_parsed_symbol_expression_destroy(out);
		return 0;
	}

	RLC_BASE_CAST(out, RlcParsedExpression)->fLast =
		rlc_parser_data_consumed_index(parser);

	return 1;
}