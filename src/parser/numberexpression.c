#include "numberexpression.h"
#include "../malloc.h"
#include <assert.h>


void rlc_parsed_number_expression_create(
	struct RlcParsedNumberExpression * this,
	size_t token_index)
{
	assert(this != NULL);

	*(enum RlcParsedExpressionType*)&RLC_BASE_CAST(this,RlcParsedExpression)->fType = kRlcParsedNumberExpression;
	this->fNumberToken = token_index;
}

void rlc_parsed_number_expression_destroy(
	struct RlcParsedNumberExpression * this)
{
	assert(this != NULL);
	assert(RLC_BASE_CAST(this,RlcParsedExpression)->fType == kRlcParsedNumberExpression);

	// do nothing, nothing to release.
}