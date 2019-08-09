#include "controllabel.h"

#include "../assert.h"

int rlc_control_label_parse(
	size_t * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);

	size_t const start = parser->fIndex;
	enum RlcParseError error_code;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBracketOpen))
	{
		goto nonfatal_failure;
	}

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

	*out = rlc_parser_data_consumed_index(parser);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBracketClose))
	{
		error_code = kRlcParseErrorExpectedBracketClose;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	parser->fIndex = start;
	return 0;
}