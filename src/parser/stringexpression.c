#include "stringexpression.h"

#include "../assert.h"
#include "../malloc.h"

static void rlc_parsed_string_expression_add(
	struct RlcParsedStringExpression * this,
	struct RlcToken const * token)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(token != NULL);

	rlc_realloc(
		(void**)&this->fTokens,
		sizeof(struct RlcToken) * ++this->fTokenCount);
	this->fTokens[this->fTokenCount-1] = *token;

	struct RlcParsedExpression * base = RLC_BASE_CAST(this, RlcParsedExpression);
	if(base->fEnd.content.start < token->content.start)
		base->fEnd = *token;
}

void rlc_parsed_string_expression_create(
	struct RlcParsedStringExpression * this,
	struct RlcToken const * first)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(first != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedStringExpression,
		*first,
		*first);

	this->fTokens = NULL;
	this->fTokenCount = 0;

	rlc_parsed_string_expression_add(this, first);
}

void rlc_parsed_string_expression_destroy(
	struct RlcParsedStringExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this,RlcParsedExpression)) == kRlcParsedStringExpression);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));

	if(this->fTokens)
	{
		rlc_free((void**)&this->fTokens);
		this->fTokenCount = 0;
	}
}

static int consume_string_literal(
	struct RlcParser * parser,
	struct RlcToken * token)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(token != NULL);

	static enum RlcTokenType const k_types[] = {
		kRlcTokStringLiteral
	};

	for(size_t i = _countof(k_types); i--;)
		if(rlc_parser_consume(
			parser,
			token,
			k_types[i]))
		{
			return 1;
		}

	return 0;
}

int rlc_parsed_string_expression_parse(
	struct RlcParsedStringExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken string;
	if(!consume_string_literal(parser, &string))
		return 0;

	rlc_parsed_string_expression_create(
		out,
		&string);

	while(consume_string_literal(parser, &string))
		rlc_parsed_string_expression_add(out, &string);

	return 1;
}

void rlc_parsed_string_expression_print(
	struct RlcParsedStringExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	for(RlcSrcIndex i = 0; i < this->fTokenCount; i++)
		rlc_src_string_print(&this->fTokens[i].content, file, out);
}