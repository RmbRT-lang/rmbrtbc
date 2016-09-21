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
		kRlcParsedNumberExpression);

	this->fNumberToken = token_index;
}

void rlc_parsed_number_expression_destroy(
	struct RlcParsedNumberExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this,RlcParsedExpression)) == kRlcParsedNumberExpression);

	// do nothing, nothing to release.
}