#include "class.h"
#include "scope.h"
#include "scopeitemgroup.h"
#include "variable.h"
#include "../parser/class.h"
#include "../assert.h"
#include "../malloc.h"

static void rlc_scoped_class_create(
	struct RlcScopedClass * this,
	struct RlcSrcFile const * file,
	struct RlcParsedClass const * parsed,
	struct RlcScopedScope * scope)
{
	this->inheritances = NULL;
	if((this->inheritanceCount = parsed->fInheritanceCount))
	{
		rlc_malloc(
			(void**)&this->inheritances,
			this->inheritanceCount * sizeof(struct RlcScopedInheritance));

		for(RlcSrcSize i = 0; i < this->inheritanceCount; i++)
		{
			this->inheritances[i].isVirtual = parsed->fInheritances[i].fVirtual;
			this->inheritances[i].visibility = parsed->fInheritances[i].fVisibility;
			rlc_scoped_symbol_create(
				&this->inheritances[i].base,
				file,
				&parsed->fInheritances[i].fBase);
		}
	}

	this->fields = NULL;
	this->fieldCount = 0;

	for(RlcSrcIndex i = 0; i < parsed->fMembers.fEntryCount; i++)
	{
		struct RlcScopedMember * member = rlc_scoped_scope_add_member(
			scope,
			file,
			parsed->fMembers.fEntries[i]);

		struct RlcScopedMemberVariable * var = RLC_DYNAMIC_CAST(
			member,
			RlcScopedMember,
			RlcScopedMemberVariable);
		if(var != NULL)
		{
			rlc_realloc(
				(void**)&this->fields,
				++this->fieldCount * sizeof(struct RlcScopedMemberVariable *));
			this->fields[this->fieldCount-1] = var;
		}
	}

	this->destructor = NULL;
	if(parsed->fHasDestructor)
	{
		struct RlcScopedMember * member = rlc_scoped_scope_add_member(
			scope,
			file,
			RLC_BASE_CAST(&parsed->fDestructor, RlcParsedMember));

		this->destructor = RLC_DERIVE_CAST(
			member,
			RlcScopedMember,
			struct RlcScopedDestructor);
	}
}

void rlc_scoped_global_class_create(
	struct RlcScopedGlobalClass * this,
	struct RlcSrcFile const * file,
	struct RlcParsedClass const * parsed,
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
		kRlcScopedClass,
		&parsed->fTemplateDecl);
	rlc_scoped_class_create(
		RLC_BASE_CAST(this, RlcScopedClass),
		file,
		parsed,
		RLC_BASE_CAST2(this, RlcScopedScopeEntry, RlcScopedScopeItem)->children);
}

static void rlc_scoped_class_destroy(
	struct RlcScopedClass * this)
{
	RLC_DASSERT(this != NULL);

	if(this->inheritanceCount)
	{
		for(RlcSrcSize i = 0; i < this->inheritanceCount; i++)
			rlc_scoped_symbol_destroy(&this->inheritances[i].base);
		rlc_free((void**)&this->inheritances);
	}
	if(this->fieldCount)
		rlc_free((void**)&this->fields);
}

void rlc_scoped_global_class_destroy(
	struct RlcScopedGlobalClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_scoped_class_destroy(
		RLC_BASE_CAST(this, RlcScopedClass));
}

void rlc_scoped_member_class_create(
	struct RlcScopedMemberClass * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberClass const * parsed,
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
		kRlcScopedMemberClass,
		&RLC_BASE_CAST(parsed, RlcParsedClass)->fTemplateDecl);

	rlc_scoped_class_create(
		RLC_BASE_CAST(this, RlcScopedClass),
		file,
		RLC_BASE_CAST(parsed, RlcParsedClass),
		RLC_BASE_CAST2(this, RlcScopedMember, RlcScopedScopeItem)->children);

}

void rlc_scoped_member_class_destroy(
	struct RlcScopedMemberClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));
}