#include "continuestatement.h"

#include "../assert.h"

void rlc_parsed_continue_statement_create(
	struct RlcParsedContinueStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedContinueStatement);
}

void rlc_parsed_continue_statement_destroy(
	struct RlcParsedContinueStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_continue_statement_parse(
	struct RlcParsedContinueStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokContinue))
		return 0;

	rlc_parsed_continue_statement_create(out);

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

void rlc_parsed_continue_statement_print(
	struct RlcParsedContinueStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fLabel.fExists)
	{
		fputs("goto ", out);
		rlc_parsed_control_label_print_name(&this->fLabel, file, out, "_continue");
		fputs(";\n", out);
	} else
	{
		fputs("continue;\n", out);
	}
}