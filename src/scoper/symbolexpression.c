#include "symbolexpression.h"
#include "../parser/symbolexpression.h"
#include "../resolver/resolver.h"
#include "../malloc.h"

#define kRlcScopedSymbolExpression kRlcParsedSymbolExpression

struct RlcScopedSymbolExpression * rlc_scoped_symbol_expression_new(
	struct RlcParsedSymbolExpression * parsed,
	struct RlcSrcFile * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	struct RlcScopedSymbolExpression * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedSymbolExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(ret, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedSymbolExpression);

	rlc_scoped_symbol_create(&ret->fName, file, &parsed->fSymbol);

	return ret;
}

void rlc_scoped_symbol_expression_delete(
	struct RlcScopedSymbolExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));
	rlc_scoped_symbol_destroy(&this->fName);
	rlc_free((void**)&this);
}