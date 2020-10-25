#include "numberexpression.h"

#include "../malloc.h"
#include "../assert.h"
#include "../scoper/number.h"


void rlc_parsed_number_expression_create(
	struct RlcParsedNumberExpression * this,
	struct RlcToken const * token)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedNumberExpression,
		*token,
		*token);

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

	struct RlcToken token;
	if(rlc_parser_consume(
		parser,
		&token,
		kRlcTokNumberLiteral))
	{
		rlc_parsed_number_expression_create(
			out,
			&token);
		return 1;
	}

	return 0;
}

void rlc_parsed_number_expression_print(
	struct RlcParsedNumberExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	struct RlcNumber number;
	rlc_number_from_token(&number, file, &this->fNumberToken);
	rlc_number_fprint(&number, out);
}