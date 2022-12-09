#include "diestatement.h"


void rlc_parsed_die_statement_create(
	struct RlcParsedDieStatement * this)
{
	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedDieStatement);
	this->fHasMessage = 0;
}

void rlc_parsed_die_statement_destroy(
	struct RlcParsedDieStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fHasMessage)
		rlc_parsed_string_expression_destroy(&this->fMessage);
	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

_Nodiscard int rlc_parsed_die_statement_parse(
	struct RlcParsedDieStatement * out,
	struct RlcParser * parser)
{
	struct RlcToken token;
	if(!rlc_parser_consume(parser, &token, kRlcTokDie))
		return 0;

	rlc_parsed_die_statement_create(out);

	out->fPosition.line = token.content.line;
	out->fPosition.column = token.content.column;

	out->fHasMessage = rlc_parsed_string_expression_parse(
		&out->fMessage, parser);

	rlc_parser_expect(parser, NULL, 1, kRlcTokSemicolon);
	return 1;
}

void rlc_parsed_die_statement_print(
	struct RlcParsedDieStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fprintf(out, "throw \"%s:%u:%u: \"",
		file->fName,
		this->fPosition.line,
		this->fPosition.column);

	if(this->fHasMessage)
	{
		rlc_parsed_string_expression_print(&this->fMessage, file, out);
		fputs(";\n", out);
	} else
		fputs("\"dying.\";\n", out);
}