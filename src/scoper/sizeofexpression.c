#include "sizeofexpression.h"
#include "../parser/sizeofexpression.h"

#include "../malloc.h"

struct RlcScopedSizeofExpression * rlc_scoped_sizeof_expression_new(
	struct RlcParsedSizeofExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	struct RlcScopedSizeofExpression * this = NULL;
	rlc_malloc((void**)&this, sizeof(struct RlcScopedSizeofExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedSizeofExpression);

	rlc_scoped_type_name_create(&this->type, file, &parsed->fType);

	return this;
}

void rlc_scoped_sizeof_expression_delete(
	struct RlcScopedSizeofExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_type_name_destroy(&this->type);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));

	rlc_free((void**)&this);
}