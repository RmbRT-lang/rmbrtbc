#include "maybeexporvar.h"
#include "expression.h"
#include "variable.h"
#include "scope.h"
#include "../malloc.h"
#include "../assert.h"

void rlc_scoped_maybe_exp_or_var_create_expression(
	struct RlcScopedMaybeExpOrVar * this,
	struct RlcSrcFile const * file,
	struct RlcParsedExpression const * expression)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(expression != NULL);

	this->exists = 1;
	this->isExpression = 1;
	this->expression = rlc_scoped_expression_new(expression, file);
}

void rlc_scoped_maybe_exp_or_var_create_empty(
	struct RlcScopedMaybeExpOrVar * this)
{
	RLC_DASSERT(this != NULL);
	this->exists = 0;
}

void rlc_scoped_maybe_exp_or_var_create_variable(
	struct RlcScopedMaybeExpOrVar * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariable const * variable,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(variable != NULL);
	RLC_DASSERT(scope != NULL);

	this->exists = 1;
	this->isExpression = 0;
	struct RlcScopedScopeEntry * entry = rlc_scoped_scope_add_entry(
		scope,
		file,
		RLC_BASE_CAST(variable, RlcParsedScopeEntry));
	this->variable = RLC_DERIVE_CAST(
		entry,
		RlcScopedScopeEntry,
		struct RlcScopedVariable);
}

void rlc_scoped_maybe_exp_or_var_destroy(
	struct RlcScopedMaybeExpOrVar * this)
{
	RLC_DASSERT(this != NULL);

	if(!this->exists)
		return;
	this->exists = 0;

	if(this->isExpression)
	{
		RLC_DASSERT(this->expression != NULL);
		rlc_scoped_expression_delete_virtual(this->expression);
	}
	else
	{
		// The variable gets destroyed with the scope.
		;
	}
}