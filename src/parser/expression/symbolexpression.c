#include "symbolexpression.h"

#include <assert.h>
#include "../../malloc.h"

struct RlcSymbolExpression * rlc_symbol_expression_create(
	struct RlcSymbol * symbol)
{
	assert(symbol != NULL);

	struct RlcSymbolExpression * this = NULL;
	rlc_malloc(
		(void**) &this,
		sizeof(struct RlcSymbolExpression));

	*(enum RlcExpression*)&this->fRlcExpression = kRlcSymbolExpression;
	this->fSymbol = *symbol;
}

void rlc_symbol_expression_destroy(
	struct RlcSymbolExpression * this)
{
	assert(this != NULL);
	assert(this->fRlcExpression == kRlcSymbolExpression);

	rlc_symbol_destroy(&this->fSymbol);
}