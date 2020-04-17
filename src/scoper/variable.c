#include "variable.h"
#include "expression.h"
#include "scopeitemgroup.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedVariable kRlcParsedVariable
#define kRlcScopedMemberVariable kRlcParsedMemberVariable

static void rlc_scoped_variable_create(
	struct RlcScopedVariable * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariable const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	if((this->hasType = parsed->fHasType))
		rlc_scoped_type_name_create(
			&this->type.name,
			file,
			&parsed->fType);
	else
		this->type.autoQualifier = parsed->fTypeQualifier;

	this->referenceType = parsed->fReferenceType;

	this->initArgs = NULL;
	if((this->initArgCount = parsed->fInitArgCount))
	{
		rlc_malloc(
			(void**)&this->initArgs,
			this->initArgCount * sizeof(struct RlcScopedExpression *));
		for(RlcSrcIndex i = 0; i < parsed->fInitArgCount; i++)
			this->initArgs[i] = rlc_scoped_expression_new(
				parsed->fInitArgs[i],
				file);
	}
}

static void rlc_scoped_variable_destroy(
	struct RlcScopedVariable * this)
{
	RLC_DASSERT(this != NULL);

	if(this->hasType)
		rlc_scoped_type_name_destroy(&this->type.name);
	this->hasType = 0;

	for(RlcSrcIndex i = this->initArgCount; i --> 0;)
		rlc_scoped_expression_delete_virtual(this->initArgs[i]);
	if(this->initArgs)
		rlc_free((void**)&this->initArgs);
}

void rlc_scoped_global_variable_create(
	struct RlcScopedGlobalVariable * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariable const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedVariable,
		&parsed->fTemplates);

	rlc_scoped_variable_create(
		RLC_BASE_CAST(this, RlcScopedVariable),
		file,
		parsed);
}

void rlc_scoped_global_variable_destroy(
	struct RlcScopedGlobalVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_variable_destroy(
		RLC_BASE_CAST(this, RlcScopedVariable));

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}

void rlc_scoped_member_variable_create(
	struct RlcScopedMemberVariable * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberVariable const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_member_create(
		RLC_BASE_CAST(this, RlcScopedMember),
		file,
		RLC_BASE_CAST(parsed, RlcParsedMember),
		parent->parent,
		kRlcScopedMemberVariable,
		&RLC_BASE_CAST(parsed, RlcParsedVariable)->fTemplates);

	rlc_scoped_variable_create(
		RLC_BASE_CAST(this, RlcScopedVariable),
		file,
		RLC_BASE_CAST(parsed, RlcParsedVariable));
}

void rlc_scoped_member_variable_destroy(
	struct RlcScopedMemberVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_variable_destroy(
		RLC_BASE_CAST(this, RlcScopedVariable));

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));
}