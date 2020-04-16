#include "thisexpression.h"
#include "../parser/thisexpression.h"

#include "../malloc.h"

struct RlcScopedThisExpression * rlc_scoped_this_expression_new(
	struct RlcParsedThisExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	struct RlcScopedThisExpression * this = NULL;
	rlc_malloc((void**)&this, sizeof(struct RlcScopedThisExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedThisExpression);

	return this;
}

void rlc_scoped_this_expression_delete(
	struct RlcScopedThisExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));
	rlc_free((void**)&this);
}