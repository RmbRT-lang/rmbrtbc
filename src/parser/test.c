#include "test.h"
#include "../assert.h"
#include "templatedecl.h"
#include "returnstatement.h"

void rlc_parsed_test_create(
	struct RlcParsedTest * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedTest,
		&kRlcSrcStringEmpty);

	this->fName = kRlcSrcStringEmpty;
	rlc_parsed_block_statement_create(&this->fBody);
}

void rlc_parsed_test_destroy(
	struct RlcParsedTest * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_block_statement_destroy(&this->fBody);
	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));
}

_Nodiscard int rlc_parsed_test_parse(
	struct RlcParsedTest * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);

	if(rlc_parsed_template_decl_exists(templates))
		return 0;

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokTest))
		return 0;

	rlc_parsed_test_create(out);

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "test", &tracer);

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokStringLiteral);
	out->fName = name.content;

	rlc_parsed_return_statement_forbid();
	if(!rlc_parsed_block_statement_parse(
		&out->fBody,
		parser))
		rlc_parser_fail(parser, "expected block statement");
	rlc_parsed_return_statement_allow();

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_test_print(
	struct RlcParsedTest * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(printer != NULL);

	if(printer->fIsTest)
	{
		FILE * out = printer->fFuncsImpl;
		fputs(
			"\n"
			"#define _return return\n"
			"#define __rl_sleep ::__rl::sleep_thread\n", out);
		fputs("__RL_TEST(", out);
		rlc_src_string_print(&this->fName, file, out);
		fputs(")", out);

		rlc_parsed_block_statement_print(&this->fBody, file, out);

		fputs(
			"\n"
			"#undef _return\n"
			"#undef __rl_sleep\n", out);
	}
}