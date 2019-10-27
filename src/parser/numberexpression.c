#include "numberexpression.h"

#include "../malloc.h"
#include "../assert.h"


void rlc_parsed_number_expression_create(
	struct RlcParsedNumberExpression * this,
	struct RlcToken const * token)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedNumberExpression,
		token->content.start);

	this->fNumberToken = *token;
}

void rlc_parsed_number_expression_destroy(
	struct RlcParsedNumberExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this,RlcParsedExpression)) == kRlcParsedNumberExpression);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_number_expression_parse(
	struct RlcParsedNumberExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	static enum RlcTokenType const k_types[] = {
		kRlcTokHexNumber,
		kRlcTokDecimalNumber,
		kRlcTokOctalNumber,
		kRlcTokCharNumber,
		kRlcTokUtf8CharNumber,
		kRlcTokUtf16CharNumber,
		kRlcTokUtf32CharNumber,
		kRlcTok0,

		kRlcTokBin16,
		kRlcTokBin32,
		kRlcTokBin64,
		kRlcTokBin128,
	};

	struct RlcToken token;
	for(size_t i = 0; i < _countof(k_types); i++)
	{
		if(rlc_parser_consume(
			parser,
			&token,
			k_types[i]))
		{
			rlc_parsed_number_expression_create(
				out,
				&token);
			return 1;
		}
	}

	return 0;
}