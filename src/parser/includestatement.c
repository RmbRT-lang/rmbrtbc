#include "includestatement.h"
#include "../assert.h"

int rlc_include_statement_parse(
	struct RlcIncludeStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokInclude))
	{
		return 0;
	}


	if(rlc_parser_data_consume(
		parser,
		kRlcTokString))
	{
		out->fIsRelative = 1;
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokCharNumber))
	{
		out->fIsRelative = 0;
	} else {
		rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedStringExpression);
		return 0;
	}
	out->fFileName = rlc_parser_data_consumed_index(parser);
	return 1;
}