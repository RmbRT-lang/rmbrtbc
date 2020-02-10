#include "numberexpression.h"
#include "../resolver/resolver.h"
#include "../assert.h"
#include "../parser/numberexpression.h"

#define kRlcScopedNumberExpression kRlcParsedNumberExpression

void rlc_scoped_number_expression_create(
	struct RlcScopedNumberExpression * this,
	struct RlcParsedNumberExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedNumberExpression);
	rlc_number_from_token(&this->value, file, &parsed->fNumberToken);
}

void rlc_scoped_number_expression_destroy(
	struct RlcScopedNumberExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));
}