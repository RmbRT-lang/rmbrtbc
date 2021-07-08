#include "nullexpression.h"

#include "../assert.h"

void rlc_parsed_null_expression_create(
	struct RlcParsedNullExpression * this,
	struct RlcToken token)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedNullExpression,
		token,
		token);
}

void rlc_parsed_null_expression_destroy(
	struct RlcParsedNullExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_null_expression_parse(
	struct RlcParsedNullExpression * out,
	struct RlcParser * parser)
{
	int result;
	struct RlcToken token;
	if((result = rlc_parser_consume(
		parser,
		&token,
		kRlcTokNull)))
	{
		rlc_parsed_null_expression_create(
			out,
			token);
	}
	return result;
}

void rlc_parsed_null_expression_print(
	struct RlcParsedNullExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	(void) this;
	(void) file;
	fprintf(out, "nullptr");
}