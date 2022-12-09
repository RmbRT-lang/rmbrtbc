#include "assertstatement.h"
#include "expression.h"
#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_assert_statement_create(
	struct RlcParsedAssertStatement * this)
{
	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedAssertStatement);

	this->fAssertion = NULL;
}

void rlc_parsed_assert_statement_destroy(
	struct RlcParsedAssertStatement * this)
{
	if(this->fAssertion)
	{
		rlc_parsed_expression_destroy_virtual(this->fAssertion);
		rlc_free((void**)&this->fAssertion);
	}

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

_Nodiscard int rlc_parsed_assert_statement_parse(
	struct RlcParsedAssertStatement * out,
	struct RlcParser * parser)
{
	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokAssert))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "assert", &tracer);

	rlc_parsed_assert_statement_create(out);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(!(out->fAssertion = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
		rlc_parser_fail(parser, "expected expression");

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);
	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);
	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_assert_statement_print(
	struct RlcParsedAssertStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fputs("__rl_assert(", out);
	rlc_parsed_expression_print(this->fAssertion, file, out);
	fputs(", (", out);

	struct RlcSrcString span = this->fAssertion->fStart.content;
	span.length = rlc_src_string_end(&this->fAssertion->fEnd.content) - span.start;

	rlc_src_string_print(&span, file, out);
	fprintf(out, "), \"%s\", %u, %u);\n", file->fName, span.line, span.column);
}