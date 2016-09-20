#include "symbolexpression.h"
#include "../assert.h"
#include "../malloc.h"

struct RlcParsedSymbolExpression * rlc_symbol_expression_create(
	struct RlcParsedSymbolExpression * this,
	struct RlcParsedSymbol const * symbol)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(symbol != NULL);

	*(enum RlcParsedExpressionType*) RLC_BASE_CAST(this, RlcParsedExpression)->fType = kRlcParsedSymbolExpression;

	this->fSymbol = *symbol;
}

void rlc_symbol_expression_destroy(
	struct RlcParsedSymbolExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->fRlcParsedExpression == kRlcParsedSymbolExpression);

	rlc_parsed_symbol_destroy(&this->fSymbol);
}