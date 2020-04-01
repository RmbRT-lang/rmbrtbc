#include "variable.h"
#include "expression.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedVariable kRlcParsedVariable

void rlc_scoped_variable_create(
	struct RlcScopedVariable * this,
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

	if(parsed->fHasType)
		rlc_scoped_type_name_create(
			&this->type.name,
			file,
			&parsed->fType);
	else
		this->type.autoQualifier = parsed->fTypeQualifier;

	this->referenceType = parsed->fReferenceType;

	this->initArgs = NULL;
	if(parsed->fInitArgCount)
	{
		rlc_malloc(
			(void**)&this->initArgs,
			parsed->fInitArgCount * sizeof(struct RlcScopedExpression *));
		for(RlcSrcIndex i = 0; i < parsed->fInitArgCount; i++)
			this->initArgs[i] = rlc_scoped_expression_new(
				parsed->fInitArgs[i],
				file);
	}
}

static inline struct RlcParsedVariable const * parsed(
	struct RlcScopedVariable const * this)
{
	return RLC_DERIVE_CAST(
		RLC_BASE_CAST(this, RlcScopedScopeEntry)->parsed,
		RlcParsedScopeEntry,
		struct RlcParsedVariable const);
}

void rlc_scoped_variable_destroy(
	struct RlcScopedVariable * this)
{
	RLC_DASSERT(this != NULL);

	if(parsed(this)->fHasType)
		rlc_scoped_type_name_destroy(&this->type.name);

	for(RlcSrcIndex i = parsed(this)->fInitArgCount; i --> 0;)
		rlc_scoped_expression_delete_virtual(this->initArgs[i]);
	if(this->initArgs)
		rlc_free((void**)&this->initArgs);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}