#include "controllabel.h"

#include "../assert.h"

int rlc_control_label_parse(
	struct RlcControlLabel * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);

	size_t const start = parser->fIndex;
	enum RlcParseError error_code;

	out->fExists = 0;
	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBracketOpen))
		return 1;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier)
	&& !rlc_parser_data_consume(
		parser,
		kRlcTokString))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	}

	out->fLabel = rlc_parser_data_consumed_index(parser);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBracketClose))
	{
		error_code = kRlcParseErrorExpectedBracketClose;
		goto failure;
	}

	out->fExists = 1;
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	parser->fIndex = start;
	out->fExists = 0;
	return 0;
}