#include "symbolchildexpression.h"
#include "../parser/symbolchildexpression.h"

#include "../malloc.h"

struct RlcScopedSymbolChildExpression * rlc_scoped_symbol_child_expression_new(
	struct RlcParsedSymbolChildExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	struct RlcScopedSymbolChildExpression * this = NULL;
	rlc_malloc((void**)&this, sizeof(struct RlcScopedSymbolChildExpression));
	rlc_scoped_expression_create(
		RLC_BASE_CAST(this, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedSymbolChildExpression);

	rlc_scoped_symbol_child_create(
		&this->symbol,
		file,
		RLC_BASE_CAST(parsed, RlcParsedSymbolChild));

	return this;
}

void rlc_scoped_symbol_child_expression_delete(
	struct RlcScopedSymbolChildExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_symbol_child_destroy(&this->symbol);
	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));

	rlc_free((void**)&this);
}