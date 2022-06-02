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
	struct RlcToken token;
	if(rlc_parser_consume(parser, &token, kRlcTokNull))
	{
		out->fNullOrBare = 1;
	} else if(rlc_parser_consume(parser, &token, kRlcTokBare))
	{
		out->fNullOrBare = 0;
	} else
		return 0;

	rlc_parsed_null_expression_create(
		out,
		token);
	return 1;
}

void rlc_parsed_null_expression_print(
	struct RlcParsedNullExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	(void) this;
	(void) file;
	fprintf(out, this->fNullOrBare ? "nullptr" : "::__rl::bare_init");
}