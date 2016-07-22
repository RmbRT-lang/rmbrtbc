#include "expression.h"
#include "symbolexpression.h"
#include "numberexpression.h"
#include "../macros.h"


#include <assert.h>



void rlc_parsed_expression_destroy(
	enum RlcParsedExpression * this)
{
	assert(this != NULL);
	assert(RLC_IN_ENUM(*this, RlcParsedExpression));

	typedef void (*destructor_t) (enum RlcParsedExpression *);

	static destructor_t const vtable[] = {
		(destructor_t)&rlc_parsed_symbol_expression_destroy,
		(destructor_t)&rlc_parsed_number_expression_destroy
	};

	assert(RLC_COVERS_ENUM(vtable, RlcParsedExpression));

	vtable[*this](this);
}