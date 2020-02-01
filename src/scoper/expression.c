#include "expression.h"

struct RlcScopedExpression * rlc_scoped_expression_new(
	struct RlcParsedExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	RLC_ASSERT(!"not implemented");
}

void rlc_scoped_expression_delete_virtual(
	struct RlcScopedExpression * this)
{
	RLC_DASSERT(this != NULL);

	RLC_ASSERT(!"not implemented");
}

void rlc_scoped_expression_create(
	struct RlcScopedExpression * this,
	struct RlcParsedExpression * parsed,
	enum RlcScopedExpressionType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type,RlcParsedExpressionType));
	RLC_DASSERT((int)type == (int)RLC_DERIVING_TYPE(parsed));

	RLC_DERIVING_TYPE(this) = type;
	this->fParsed = parsed;
}

void rlc_scoped_expression_destroy_base(
	struct RlcScopedExpression * this)
{
	RLC_DASSERT(this != NULL);

	this->fParsed = NULL;
}