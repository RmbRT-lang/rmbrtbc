#include "numberexpression.h"
#include "../malloc.h"
#include <assert.h>


void rlc_parsed_number_expression_create(
	struct RlcParsedNumberExpression * this,
	size_t token_index)
{
	assert(this != NULL);

	*(enum RlcParsedExpression*)&this->fRlcParsedExpression = kRlcParsedNumberExpression;
	this->fNumberToken = token_index;
}

void rlc_parsed_number_expression_destroy(
	struct RlcParsedNumberExpression * this)
{
	assert(this != NULL);
	assert(this->fRlcParsedExpression == kRlcParsedNumberExpression);

	// do nothing, nothing to release.
}