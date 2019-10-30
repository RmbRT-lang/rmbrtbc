#include "castexpression.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_cast_expression_create(
	struct RlcParsedCastExpression * this,
	size_t first)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedCastExpression,
		first);

	rlc_parsed_type_name_create(&this->fType);
	this->fValue = NULL;
}


void rlc_parsed_cast_expression_destroy(
	struct RlcParsedCastExpression * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fValue)
	{
		rlc_parsed_expression_destroy_virtual(this->fValue);
		rlc_free((void**)&this->fValue);
	}

	rlc_parsed_type_name_destroy(&this->fType);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_cast_expression_parse(
	struct RlcParsedCastExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);


	rlc_parsed_cast_expression_create(out, rlc_parser_index(parser));

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokLess))
		return 0;

	rlc_parsed_type_name_parse(
		&out->fType,
		parser);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokGreater);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	out->fValue = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);

	return 1;
}
