#include "expression.h"
#include "symbolexpression.h"
#include "numberexpression.h"
#include "../macros.h"
#include "../assert.h"


void rlc_parsed_expression_destroy(
	struct RlcParsedExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(this->fType, RlcParsedExpression));

	typedef void (*destructor_t) (struct RlcParsedExpression *);

	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_parsed_symbol_expression_destroy,
		(destructor_t)&rlc_parsed_number_expression_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedExpression), "ill sized vtable.");

	k_vtable[this->fType](this);
}