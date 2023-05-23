#include "baseexpression.h"

void rlc_parsed_base_expression_create(
	struct RlcParsedBaseExpression * this,
	struct RlcToken start,
	struct RlcToken end)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedBaseExpression,
		start,
		end);

	this->fIsName = 0;
}

void rlc_parsed_base_expression_destroy(
	struct RlcParsedBaseExpression * this)
{
	if(this->fIsName)
		rlc_parsed_symbol_destroy(&this->fBase.fName);
	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_base_expression_parse(
	struct RlcParsedBaseExpression * out,
	struct RlcParser * parser)
{
	struct RlcToken start;
	if(!rlc_parser_is_ahead(parser, kRlcTokParentheseOpen)
	|| !rlc_parser_consume(parser, &start, kRlcTokGreater))
		return 0;

	rlc_parser_skip(parser);

	struct RlcToken end;

	int isIndex;
	if((isIndex = rlc_parser_consume(
		parser,
		&out->fBase.fIndex,
		kRlcTokNumberLiteral)))
	{
		rlc_parser_fail(parser,
			"sorry but the bootstrap compiler is too stupid to look up the base class name by index! Please specify the name directly.");
	} else
	{
		if(!rlc_parsed_symbol_parse(&out->fBase.fName, parser))
			rlc_parser_fail(parser, "expected index or base class name");
	}
	rlc_parser_expect(parser, &end, 1, kRlcTokParentheseClose);
	rlc_parsed_base_expression_create(out, start, end);
	out->fIsName = !isIndex;

	return 1;
}

extern int rlc_const_context;

void rlc_parsed_base_expression_print(
	struct RlcParsedBaseExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->fIsName);
	fputs(" static_cast<", out);
	rlc_parsed_symbol_print_no_template(&this->fBase.fName, file, out);
	if(rlc_const_context)
		fputs(" const", out);
	fputs(" &>(*this)", out);
}