#include "stringexpression.h"

#include "../assert.h"

void rlc_parsed_string_expression_create(
	struct RlcParsedStringExpression * this,
	size_t first)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedStringExpression,
		first);

	this->fStartToken = first;
	this->fTokenCount = 0;
}

void rlc_parsed_string_expression_destroy(
	struct RlcParsedStringExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this,RlcParsedExpression)) == kRlcParsedStringExpression);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

static int consume_string_literal(
	struct RlcParserData * parser)
{
	static enum RlcTokenType const k_types[] = {
		kRlcTokString,
		kRlcTokUtf8String,
		kRlcTokUtf16leString,
		kRlcTokUtf16beString,
		kRlcTokUtf32leString,
		kRlcTokUtf32beString
	};

	for(size_t i = _countof(k_types); i--;)
		if(rlc_parser_data_consume(
			parser,
			k_types[i]))
		{
			return 1;
		}

	return 0;
}

int rlc_parsed_string_expression_parse(
	struct RlcParsedStringExpression * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!consume_string_literal(parser))
		return 0;

	rlc_parsed_string_expression_create(
		out,
		rlc_parser_data_consumed_index(parser));

	out->fTokenCount = 1;

	while(consume_string_literal(parser))
		++out->fTokenCount;

	RLC_BASE_CAST(out, RlcParsedExpression)->fLast =
		rlc_parser_data_consumed_index(parser);

	return 1;
}