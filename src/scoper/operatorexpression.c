#include "operatorexpression.h"
#include "../assert.h"
#include "../malloc.h"

struct RlcScopedOperatorExpression * rlc_scoped_operator_expression_new(
	struct RlcParsedOperatorExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	struct RlcScopedOperatorExpression * this = NULL;
	rlc_malloc((void**)&this, sizeof(struct RlcScopedOperatorExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedOperatorExpression);

	this->arguments = NULL;

	if(parsed->fExpressionCount)
	{
		rlc_malloc(
			(void**)&this->arguments,
			parsed->fExpressionCount * sizeof(void *));

		for(RlcSrcIndex i = 0; i < parsed->fExpressionCount; i++)
			this->arguments[i] = rlc_scoped_expression_new(
				parsed->fExpressions[i],
				file);
	}

	return this;
}

void rlc_scoped_operator_expression_delete(
	struct RlcScopedOperatorExpression * this)
{
	RLC_DASSERT(this != NULL);

	for(RlcSrcIndex i = rlc_scoped_operator_expression_argument_count(this); i--;)
		rlc_scoped_expression_delete_virtual(this->arguments[i]);

	rlc_free((void**)&this->arguments);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));

	rlc_free((void**)&this);
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