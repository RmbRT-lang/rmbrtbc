#include "operatorexpression.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_scoped_operator_expression_create(
	struct RlcScopedOperatorExpression * this,
	struct RlcParsedOperatorExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedOperatorExpression);

	this->arguments = NULL;
}

void rlc_scoped_operator_expression_destroy(
	struct RlcScopedOperatorExpression * this)
{
	RLC_DASSERT(this != NULL);

	for(RlcSrcIndex i = rlc_scoped_operator_expression_argument_count(this); i--;)
		rlc_scoped_expression_delete_virtual(this->arguments[i]);

	rlc_free((void**)&this->arguments);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));
}

enum RlcOperator rlc_scoped_operator_expression_operator(
	struct RlcScopedOperatorExpression const * this)
{
	RLC_DASSERT(this != NULL);

	return RLC_DERIVE_CAST(
		RLC_BASE_CAST(this, RlcScopedExpression)->fParsed,
		RlcParsedExpression,
		struct RlcParsedOperatorExpression const)->fOperator;
}

RlcSrcSize rlc_scoped_operator_expression_argument_count(
	struct RlcScopedOperatorExpression const * this)
{
	RLC_DASSERT(this != NULL);

	return RLC_DERIVE_CAST(
		RLC_BASE_CAST(this, RlcScopedExpression)->fParsed,
		RlcParsedExpression,
		struct RlcParsedOperatorExpression const)->fExpressionCount;
}