#include "includestatement.h"
#include "../assert.h"

int rlc_parsed_include_statement_parse(
	struct RlcParsedIncludeStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokInclude))
		return 0;
	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "include statement", &tracer);

	out->fIsRelative = kRlcTokStringLiteral == rlc_parser_expect(
		parser,
		&out->fFileName,
		2,
		kRlcTokStringLiteral,
		kRlcTokCharacterLiteral);

	rlc_parser_untrace(parser, &tracer);
	return 1;
}