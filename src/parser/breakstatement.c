#include "breakstatement.h"

#include "../assert.h"

void rlc_parsed_break_statement_create(
	struct RlcParsedBreakStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedBreakStatement);

	rlc_control_label_create(&this->fLabel);
}

void rlc_parsed_break_statement_destroy(
	struct RlcParsedBreakStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));

	rlc_control_label_destroy(&this->fLabel);
}

int rlc_parsed_break_statement_parse(
	struct RlcParsedBreakStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;
	enum RlcParseError error_code;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBreak))
		return 0;

	rlc_parsed_break_statement_create(out);

	if(!rlc_control_label_parse(
		&out->fLabel,
		parser))
	{
		error_code = kRlcParseErrorExpectedControlLabel;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	parser->fIndex = start;
	rlc_parsed_break_statement_destroy(out);
	return 0;
}