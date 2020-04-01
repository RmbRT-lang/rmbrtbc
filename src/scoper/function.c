#include "expression.h"
#include "function.h"
#include "scope.h"
#include "statement.h"
#include "variable.h"
#include "../parser/function.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedFunction kRlcParsedFunction

static inline struct RlcParsedFunction const * parsed(
	struct RlcScopedFunction const * this)
{
	return RLC_DERIVE_CAST(
		RLC_BASE_CAST(this, RlcScopedScopeEntry)->parsed,
		RlcParsedScopeEntry,
		struct RlcParsedFunction const);
}

void rlc_scoped_function_create(
	struct RlcScopedFunction * this,
	struct RlcSrcFile const * file,
	struct RlcParsedFunction const * parsed,
	struct RlcScopedScopeItemGroup * group)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(group != NULL);

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		group,
		kRlcScopedFunction,
		&parsed->fTemplates);

	this->arguments = NULL;
	if(parsed->fArgumentCount)
		rlc_malloc(
			(void**)&this->arguments,
			parsed->fArgumentCount * sizeof(struct RlcScopedVariable *));

	for(RlcSrcIndex i = 0; i < parsed->fArgumentCount; i++)
	{
		struct RlcScopedScopeEntry * it = rlc_scoped_scope_add_entry(
			RLC_BASE_CAST2(this, RlcScopedScopeEntry, RlcScopedScopeItem)->children,
			file,
			RLC_BASE_CAST(&parsed->fArguments[i], RlcParsedScopeEntry));

		this->arguments[i] = RLC_DERIVE_CAST(
			it,
			RlcScopedScopeEntry,
			struct RlcScopedVariable);
	}

	if(parsed->fHasReturnType)
		rlc_scoped_type_name_create(
			&this->returnType,
			file,
			&parsed->fReturnType);

	if(parsed->fHasBody)
	{
		if(parsed->fIsShortHandBody)
			this->body.expression = rlc_scoped_expression_new(
				parsed->fReturnValue,
				file);
		else
			this->body.statement = rlc_scoped_statement_new(
				file,
				RLC_BASE_CAST(&parsed->fBodyStatement, RlcParsedStatement));
	}
}

void rlc_scoped_function_destroy(
	struct RlcScopedFunction * this)
{
	RLC_DASSERT(this != NULL);

	struct RlcParsedFunction const * p = parsed(this);

	if(p->fArgumentCount)
		rlc_free((void**)&this->arguments);

	if(p->fHasReturnType)
		rlc_scoped_type_name_destroy(&this->returnType);

	if(p->fHasBody)
	{
		if(p->fIsShortHandBody)
			rlc_scoped_expression_delete_virtual(this->body.expression);
		else
			rlc_scoped_statement_delete(this->body.statement);
	}

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}