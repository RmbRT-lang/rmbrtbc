#include "numberexpression.h"

#include "../malloc.h"
#include "../assert.h"


void rlc_parsed_number_expression_create(
	struct RlcParsedNumberExpression * this,
	size_t token_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedNumberExpression,
		token_index);

	this->fNumberToken = token_index;
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
	struct RlcParserData * parser)
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

	for(size_t i = 0; i < _countof(k_types); i++)
	{
		if(rlc_parser_data_consume(
			parser,
			k_types[i]))
		{
			rlc_parsed_number_expression_create(
				out,
				rlc_parser_data_consumed_index(parser));
			return 1;
		}
	}

	return 0;
}