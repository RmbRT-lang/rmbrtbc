#include "numberexpression.h"
#include "../resolver/resolver.h"
#include "../assert.h"
#include "../parser/numberexpression.h"
#include "../malloc.h"

#define kRlcScopedNumberExpression kRlcParsedNumberExpression

struct RlcScopedNumberExpression * rlc_scoped_number_expression_new(
	struct RlcParsedNumberExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	struct RlcScopedNumberExpression * this = NULL;
	rlc_malloc((void**)&this, sizeof(struct RlcScopedNumberExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedNumberExpression);
	rlc_number_from_token(&this->value, file, &parsed->fNumberToken);

	return this;
}

void rlc_scoped_number_expression_delete(
	struct RlcScopedNumberExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));

	rlc_free((void**)&this);
}