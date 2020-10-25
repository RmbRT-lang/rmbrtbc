#include "characterexpression.h"

void rlc_parsed_character_expression_create(
	struct RlcParsedCharacterExpression * this,
	struct RlcToken const * token)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(token != NULL);
	RLC_DASSERT(token->type == kRlcTokCharacterLiteral);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedCharacterExpression,
		*token,
		*token);
	this->fToken = *token;
}

void rlc_parsed_character_expression_destroy(
	struct RlcParsedCharacterExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_character_expression_parse(
	struct RlcParsedCharacterExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken token;
	if(!rlc_parser_consume(
		parser,
		&token,
		kRlcTokCharacterLiteral))
		return 0;

	rlc_parsed_character_expression_create(out, &token);
	return 1;
}

void rlc_parsed_character_expression_print(
	struct RlcParsedCharacterExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	rlc_src_string_print(&this->fToken.content, file, out);
}