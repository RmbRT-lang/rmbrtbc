#include "class.h"
#include "scope.h"
#include "../parser/class.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedClass kRlcParsedClass

struct RlcScopedClass * rlc_scoped_class_new(
	struct RlcSrcFile const * file,
	struct RlcParsedClass const * parsed,
	struct RlcScopedScopeItemGroup * group)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(group != NULL);

	struct RlcScopedClass * this = NULL;
	rlc_malloc((void**)&this, sizeof(struct RlcScopedClass));

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		group,
		kRlcScopedClass,
		&parsed->fTemplateDecl);

	struct RlcScopedScopeItem * thisAsItem =
		RLC_BASE_CAST2(this, RlcScopedScopeEntry, RlcScopedScopeItem);
	for(RlcSrcIndex i = 0; i < parsed->fMembers.fEntryCount; i++)
	{
		rlc_scoped_scope_add_member(
			thisAsItem->children,
			file,
			parsed->fMembers.fEntries[i]);
	}

	return this;
}

void rlc_scoped_class_delete(
	struct RlcScopedClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_free((void**)&this);
}