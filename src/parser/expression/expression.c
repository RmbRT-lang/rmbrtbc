#include "expression.h"
#include "symbolexpression.h"
#include "numberexpression.h"

#include <assert.h>



void rlc_expression_destroy(
	enum RlcExpression * this)
{
	assert(this != NULL);
	assert(RLC_IN_ENUM(*this, RlcExpression));

	typedef void (*destructor_t) (enum RlcExpression *);

	static destructor_t const k_destructors[RLC_COUNT(RlcExpression)] = {
		(destructor_t)&rlc_symbol_expression_destroy,
		(destructor_t)&rlc_number_expression_destroy
	};

	k_destructors[*this](this);
}