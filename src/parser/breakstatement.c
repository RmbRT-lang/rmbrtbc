#include "breakstatement.h"

#include "../assert.h"

void rlc_parsed_break_statement_create(
	struct RlcParsedBreakStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedBreakStatement);
}

void rlc_parsed_break_statement_destroy(
	struct RlcParsedBreakStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_break_statement_parse(
	struct RlcParsedBreakStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBreak))
		return 0;

	rlc_parsed_break_statement_create(out);

	rlc_parsed_control_label_parse(
		&out->fLabel,
		parser);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	return 1;
}

void rlc_parsed_break_statement_print(
	struct RlcParsedBreakStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fLabel.fExists)
	{
		fputs("goto ", out);
		rlc_parsed_control_label_print_name(&this->fLabel, file, out, "_break");
		fputs(";\n", out);
	} else
	{
		fputs("break;\n", out);
	}
}