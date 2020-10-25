#include "symbolchildexpression.h"
#include "../assert.h"


void rlc_parsed_symbol_child_expression_create(
	struct RlcParsedSymbolChildExpression * this,
	struct RlcToken first)
{
	RLC_DASSERT(this != NULL);

	struct RlcToken last = {
		RLC_BASE_CAST(this, RlcParsedSymbolChild)->fName,
		kRlcTokIdentifier
	};

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedSymbolChildExpression,
		first,
		last);
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

	if(!rlc_parser_current(parser))
		return 0;

	struct RlcToken first = *rlc_parser_current(parser);
	if(!rlc_parsed_symbol_child_parse(
		RLC_BASE_CAST(out, RlcParsedSymbolChild),
		parser,
		1))
	{
		return 0;
	}

	rlc_parsed_symbol_child_expression_create(
		out,
		first);

	return 1;
}

void rlc_parsed_symbol_child_expression_print(
	struct RlcParsedSymbolChildExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	rlc_parsed_symbol_child_print(
		RLC_BASE_CAST(this, RlcParsedSymbolChild),
		file,
		out,
		1);
}