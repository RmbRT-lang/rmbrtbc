#include "expression.h"
#include "symbolexpression.h"
#include "numberexpression.h"
#include "typenameexpression.h"
#include "../macros.h"
#include "../assert.h"


void rlc_parsed_expression_create(
	struct RlcParsedExpression * this,
	enum RlcParsedExpressionType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedExpressionType));

	RLC_DERIVING_TYPE(this) = type;
	this->fFirst = 0;
	this->fLast = 0;
}

void rlc_parsed_expression_destroy(
	struct RlcParsedExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedExpressionType));

	typedef void (*destructor_t) (struct RlcParsedExpression *);

	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_parsed_symbol_expression_destroy,
		(destructor_t)&rlc_parsed_number_expression_destroy,
		(destructor_t)&rlc_parsed_type_name_expression_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedExpressionType), "ill sized vtable.");

	k_vtable[RLC_DERIVING_TYPE(this)](this);
}