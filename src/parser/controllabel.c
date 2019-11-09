#include "controllabel.h"

#include "../assert.h"

void rlc_control_label_parse(
	struct RlcControlLabel * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);

	out->fExists = 0;
	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBracketOpen))
		return;

	rlc_parser_expect(
		parser,
		&out->fLabel,
		2,
		kRlcTokIdentifier, kRlcTokStringLiteral);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBracketClose);

	out->fExists = 1;
}