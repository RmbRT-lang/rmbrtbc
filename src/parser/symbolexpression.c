#include "symbolexpression.h"
#include "../assert.h"

void rlc_parsed_symbol_expression_create(
	struct RlcParsedSymbolExpression * this,
	struct RlcToken first)
{
	RLC_DASSERT(this != NULL);

	struct RlcToken last = {
		this->fSymbol.fChildren[this->fSymbol.fChildCount-1].fName,
		kRlcTokIdentifier
	};

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedSymbolExpression,
		first,
		last);
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
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_current(parser))
		return 0;

	struct RlcToken start = *rlc_parser_current(parser);

	if(!rlc_parsed_symbol_parse(
		&out->fSymbol,
		parser,
		1))
	{
		return 0;
	}

	rlc_parsed_symbol_expression_create(
		out,
		start);

	return 1;
}

void rlc_parsed_symbol_expression_print(
	struct RlcParsedSymbolExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	rlc_parsed_symbol_print(&this->fSymbol, file, out);
}