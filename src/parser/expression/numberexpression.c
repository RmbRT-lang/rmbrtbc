#include "numberexpression.h"
#include "../../malloc.h"
#include <assert.h>


struct RlcNumberExpression * rlc_number_expression_create(
	size_t token_index)
{
	struct RlcNumberExpression * this = NULL;
	rlc_malloc(
		(void**)&this,
		sizeof(struct RlcNumberExpression));

	*(enum RlcExpression*)&this->fRlcExpression = kRlcNumberExpression;
	this->fNumberToken = token_index;
}

void rlc_number_expression_destroy(
	struct RlcNumberExpression * this)
{
	assert(this != NULL);
	assert(this->fRlcExpression == kRlcNumberExpression);

	// do nothing, nothing to release.
}