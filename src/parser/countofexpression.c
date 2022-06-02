#include "countofexpression.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_countof_expression_create(
	struct RlcParsedCountofExpression * this,
	struct RlcToken start,
	struct RlcToken end)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedCountofExpression,
		start,
		end);
}

void rlc_parsed_countof_expression_destroy(
	struct RlcParsedCountofExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));

	rlc_parsed_symbol_destroy(&this->fType);
}

int rlc_parsed_countof_expression_parse(
	struct RlcParsedCountofExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken start;

	if(!rlc_parser_consume(
		parser,
		&start,
		kRlcTokTripleHash))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "count-of expression", &tracer);

	if(!rlc_parsed_symbol_parse(&out->fType, parser))
		rlc_parser_fail(parser, "expected type name");

	struct RlcToken last = {
		out->fType.fChildren[out->fType.fChildCount-1].fName,
		kRlcTokIdentifier
	};

	rlc_parsed_countof_expression_create(out, start, last);

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_countof_expression_print(
	struct RlcParsedCountofExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fputs("(", out);
	rlc_parsed_symbol_print(&this->fType, file, out);
	fputs("::size())", out);
}