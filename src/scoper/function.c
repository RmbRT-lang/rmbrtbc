#include "expression.h"
#include "function.h"
#include "scope.h"
#include "scopeitemgroup.h"
#include "statement.h"
#include "variable.h"
#include "../parser/function.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedFunction kRlcParsedFunction
#define kRlcScopedMemberFunction kRlcParsedMemberFunction

static void rlc_scoped_function_create(
	struct RlcScopedFunction * this,
	struct RlcSrcFile const * file,
	struct RlcParsedFunction const * parsed,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(scope != NULL);

	this->arguments = NULL;
	if(parsed->fArgumentCount)
		rlc_malloc(
			(void**)&this->arguments,
			parsed->fArgumentCount * sizeof(struct RlcScopedVariable *));

	for(RlcSrcIndex i = 0; i < parsed->fArgumentCount; i++)
	{
		struct RlcScopedScopeEntry * it = rlc_scoped_scope_add_entry(
			scope,
			file,
			RLC_BASE_CAST(&parsed->fArguments[i], RlcParsedScopeEntry));

		this->arguments[i] = RLC_DERIVE_CAST(
			it,
			RlcScopedScopeEntry,
			struct RlcScopedGlobalVariable);
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

static void rlc_scoped_function_destroy(
	struct RlcScopedFunction * this)
{
	RLC_DASSERT(this != NULL);

	if(this->argumentCount)
		rlc_free((void**)&this->arguments);

	if(this->hasReturnType)
		rlc_scoped_type_name_destroy(&this->returnType);

	if(this->hasBody)
	{
		if(this->isShortHandBody)
			rlc_scoped_expression_delete_virtual(this->body.expression);
		else
			rlc_scoped_statement_delete(this->body.statement);
	}
}


void rlc_scoped_global_function_create(
	struct RlcScopedGlobalFunction * this,
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

	rlc_scoped_function_create(
		RLC_BASE_CAST(this, RlcScopedFunction),
		file,
		parsed,
		RLC_BASE_CAST2(this, RlcScopedScopeEntry, RlcScopedScopeItem)->children);
}

void rlc_scoped_global_function_destroy(
	struct RlcScopedGlobalFunction * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_function_destroy(
		RLC_BASE_CAST(this, RlcScopedFunction));

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}

void rlc_scoped_member_function_create(
	struct RlcScopedMemberFunction * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberFunction const * parsed,
	struct RlcScopedScopeItemGroup * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(scope != NULL);

	rlc_scoped_member_create(
		RLC_BASE_CAST(this, RlcScopedMember),
		file,
		RLC_BASE_CAST(parsed, RlcParsedMember),
		scope->parent,
		kRlcScopedMemberFunction,
		&RLC_BASE_CAST(parsed, RlcParsedFunction)->fTemplates);

	rlc_scoped_function_create(
		RLC_BASE_CAST(this, RlcScopedFunction),
		file,
		RLC_BASE_CAST(parsed, RlcParsedFunction),
		RLC_BASE_CAST2(
			this,
			RlcScopedMember,
			RlcScopedScopeItem)->children);
}

void rlc_scoped_member_function_destroy(
	struct RlcScopedMemberFunction * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));

	rlc_scoped_function_destroy(
		RLC_BASE_CAST(this, RlcScopedFunction));
}