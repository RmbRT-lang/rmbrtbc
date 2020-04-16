#include "castexpression.h"
#include "../parser/castexpression.h"

#include "../malloc.h"

struct RlcScopedCastExpression * rlc_scoped_cast_expression_new(
	struct RlcParsedCastExpression const * parsed,
	struct RlcSrcFile const * file)
{
	struct RlcScopedCastExpression * this = NULL;
	rlc_malloc((void**)&this, sizeof(struct RlcScopedCastExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedCastExpression);

	rlc_scoped_type_name_create(&this->type, file, &parsed->fType);

	this->value = rlc_scoped_expression_new(parsed->fValue, file);

	return this;
}

void rlc_scoped_cast_expression_delete(
	struct RlcScopedCastExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));

	rlc_scoped_type_name_destroy(&this->type);
	rlc_scoped_expression_delete_virtual(this->value);
	rlc_free((void**)&this);
}