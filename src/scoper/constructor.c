#include "constructor.h"
#include "scopeitemgroup.h"
#include "scope.h"
#include "variable.h"
#include "expression.h"
#include "../parser/constructor.h"

#include "../malloc.h"
#include "../assert.h"

#define kRlcScopedConstructor kRlcParsedConstructor

static void rlc_scoped_initialiser_create(
	struct RlcScopedInitialiser * this,
	struct RlcSrcFile const * file,
	struct RlcParsedInitialiser * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_identifier_create(
		&this->member,
		file,
		&parsed->fMember);

	this->arguments = NULL;
	if((this->argumentCount = parsed->fArgumentCount))
	{
		rlc_malloc(
			(void**)&this->arguments,
			this->argumentCount * sizeof(struct RlcScopedExpression *));

		for(RlcSrcIndex i = 0; i < this->argumentCount; i++)
			this->arguments[i] = rlc_scoped_expression_new(
				parsed->fArguments[i],
				file);
	}
}

static void rlc_scoped_initialiser_destroy(
	struct RlcScopedInitialiser * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_identifier_destroy(&this->member);

	if(this->argumentCount)
	{
		rlc_free((void**)&this->arguments);
		this->argumentCount = 0;
	}
}

void rlc_scoped_constructor_create(
	struct RlcScopedConstructor * this,
	struct RlcSrcFile const * file,
	struct RlcParsedConstructor const * parsed,
	struct RlcScopedScopeItemGroup * group)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(group != NULL);

	rlc_scoped_member_create(
		RLC_BASE_CAST(this, RlcScopedMember),
		file,
		RLC_BASE_CAST(parsed, RlcParsedMember),
		group->parent,
		kRlcScopedConstructor,
		&parsed->fTemplates);

	this->arguments = NULL;
	if((this->argumentCount = parsed->fArgumentCount))
	{
		rlc_malloc(
			(void**)&this->arguments,
			this->argumentCount * sizeof(struct RlcScopedGlobalVariable *));

		for(RlcSrcIndex i = 0; i < this->argumentCount; i++)
		{
			struct RlcScopedScopeEntry * entry = rlc_scoped_scope_add_entry(
				RLC_BASE_CAST2(
					this,
					RlcScopedMember,
					RlcScopedScopeItem)->children,
				file,
				RLC_BASE_CAST(&parsed->fArguments[i], RlcParsedScopeEntry));

			this->arguments[i] = RLC_DERIVE_CAST(
				entry,
				RlcScopedScopeEntry,
				struct RlcScopedGlobalVariable);
		}
	}


	this->initialisers = NULL;
	if((this->initialiserCount = parsed->fInitialiserCount))
	{
		rlc_malloc(
			(void**)&this->initialisers,
			this->initialiserCount * sizeof(struct RlcScopedInitialiser));

		for(RlcSrcIndex i = 0; i < this->initialiserCount; i++)
			rlc_scoped_initialiser_create(
				&this->initialisers[i],
				file,
				&parsed->fInitialisers[i]);
	}

	if((this->isDefinition = parsed->fIsDefinition))
	{
		rlc_scoped_block_statement_create(
			&this->body,
			file,
			&parsed->fBody,
			RLC_BASE_CAST2(
				this,
				RlcScopedMember,
				RlcScopedScopeItem)->children);
	}
	this->isInline = parsed->fIsInline;

}

void rlc_scoped_constructor_destroy(
	struct RlcScopedConstructor * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));

	if(this->argumentCount)
	{
		rlc_free((void**)&this->arguments);
		this->argumentCount = 0;
	}

	if(this->initialiserCount)
	{
		for(RlcSrcIndex i = 0; i < this->initialiserCount; i++)
			rlc_scoped_initialiser_destroy(&this->initialisers[i]);

		rlc_free((void**)&this->initialisers);
	}
}