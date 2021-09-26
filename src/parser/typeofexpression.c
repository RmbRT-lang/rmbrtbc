#include "typeofexpression.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_typeof_expression_create(
	struct RlcParsedTypeofExpression * this,
	struct RlcToken start,
	struct RlcToken end)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedTypeofExpression,
		start,
		end);
}

void rlc_parsed_typeof_expression_destroy(
	struct RlcParsedTypeofExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));

	if(this->fIsTypeArg)
		rlc_parsed_type_name_destroy(&this->fType);
	else if(this->fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fExpression);
		rlc_free((void**)&this->fExpression);
	}
}

int rlc_parsed_typeof_expression_parse(
	struct RlcParsedTypeofExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken start;

	if(!rlc_parser_consume(parser, &start, kRlcTokType))
		return 0;

	struct RlcParserTracer ctx;
	rlc_parser_trace(parser, "type expression", &ctx);

	if(!(out->fIsTypeArg = rlc_parser_consume(parser, NULL, kRlcTokType)))
		out->fIsStatic = rlc_parser_consume(parser, NULL, kRlcTokStatic);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(out->fIsTypeArg)
	{
		if(!rlc_parsed_type_name_parse(&out->fType, parser, 0))
			rlc_parser_fail(parser, "expected type name");
	} else
	{
		if(!(out->fExpression = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType))))
		{
			rlc_parser_fail(parser, "expected expression");
		}
	}

	struct RlcToken end;
	rlc_parser_expect(
		parser,
		&end,
		1,
		kRlcTokParentheseClose);

	rlc_parsed_typeof_expression_create(out, start, end);
	rlc_parser_untrace(parser, &ctx);
	return 1;
}

void rlc_parsed_typeof_expression_print(
	struct RlcParsedTypeofExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fIsTypeArg)
	{
		fputs("::__rl::type_name<", out);
		rlc_parsed_type_name_print(&this->fType, file, out);
		fputs(">()", out);
	}
	else
	{
		fputs(this->fIsStatic
			? "::__rl::type_name"
			: "::__rl::deriving_type_name",
			out);
		fputc('(', out);
		rlc_parsed_expression_print(this->fExpression, file, out);
		fputc(')', out);
	}
}