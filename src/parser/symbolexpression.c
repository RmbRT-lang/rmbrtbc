#include "symbolexpression.h"

#include <assert.h>
#include "../../malloc.h"

struct RlcParsedSymbolExpression * rlc_symbol_expression_create(
	struct RlcParsedSymbol * symbol)
{
	assert(symbol != NULL);

	struct RlcParsedSymbolExpression * this = NULL;
	rlc_malloc(
		(void**) &this,
		sizeof(struct RlcParsedSymbolExpression));

	*(enum RlcParsedExpression*)&this->fRlcParsedExpression = kRlcParsedSymbolExpression;
	this->fSymbol = *symbol;
}

void rlc_symbol_expression_destroy(
	struct RlcParsedSymbolExpression * this)
{
	assert(this != NULL);
	assert(this->fRlcParsedExpression == kRlcParsedSymbolExpression);

	rlc_symbol_destroy(&this->fSymbol);
}