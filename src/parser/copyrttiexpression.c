#include "copyrttiexpression.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_copy_rtti_expression_create(
	struct RlcParsedCopyRttiExpression * this,
	struct RlcToken start,
	struct RlcToken end)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedCopyRttiExpression,
		start,
		end);
}

void rlc_parsed_copy_rtti_expression_destroy(
	struct RlcParsedCopyRttiExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));

	if(this->fSource)
	{
		rlc_parsed_expression_destroy_virtual(this->fSource);
		rlc_free((void**)&this->fSource);
	}
	if (this->fDest)
	{
		rlc_parsed_expression_destroy_virtual(this->fDest);
		rlc_free((void**)&this->fDest);
	}
}

int rlc_parsed_copy_rtti_expression_parse(
	struct RlcParsedCopyRttiExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken start;

	if(!rlc_parser_consume(
		parser,
		&start,
		kRlcTokCopyRtti))
		return 0;

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(!(out->fSource = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		rlc_parser_fail(parser, "expected expression");
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokComma);

	if(!(out->fDest = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		rlc_parser_fail(parser, "expected expression");
	}

	struct RlcToken end;
	rlc_parser_expect(
		parser,
		&end,
		1,
		kRlcTokParentheseClose);

	rlc_parsed_copy_rtti_expression_create(out, start, end);
	return 1;
}

void rlc_parsed_copy_rtti_expression_print(
	struct RlcParsedCopyRttiExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fputs("::__rl::copy_rtti(", out);
	rlc_parsed_expression_print(this->fSource, file, out);
	fputs(", ", out);
	rlc_parsed_expression_print(this->fDest, file, out);
	fputc(')', out);
}