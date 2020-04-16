#include "union.h"
#include "scope.h"
#include "scopeitemgroup.h"
#include "union.h"
#include "variable.h"
#include "../parser/union.h"

#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedGlobalUnion kRlcParsedUnion
#define kRlcScopedMemberUnion kRlcParsedMemberUnion

static void rlc_scoped_union_create(
	struct RlcScopedUnion * this,
	struct RlcSrcFile const * file,
	struct RlcParsedUnion const * parsed,
	struct RlcScopedScope * members)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(members != NULL);

	this->fields = NULL;
	this->fieldCount = 0;

	for(RlcSrcIndex i = 0; i < parsed->fMembers.fEntryCount; i++)
	{
		struct RlcScopedMember * member = rlc_scoped_scope_add_member(
			members,
			file,
			parsed->fMembers.fEntries[i]);

		if(RLC_DERIVING_TYPE(member) == kRlcParsedMemberVariable)
		{
			rlc_realloc(
				(void**)&this->fields,
				++this->fieldCount * sizeof(struct RlcScopedMemberVariable *));
			this->fields[this->fieldCount-1] = RLC_DERIVE_CAST(
				member,
				RlcScopedMember,
				struct RlcScopedMemberVariable);
		}
	}
}

static void rlc_scoped_union_destroy(
	struct RlcScopedUnion * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fieldCount)
	{
		rlc_free((void**)&this->fields);
		this->fieldCount = 0;
	}
}

void rlc_scoped_global_union_create(
	struct RlcScopedGlobalUnion * this,
	struct RlcSrcFile const * file,
	struct RlcParsedUnion const * parsed,
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
		kRlcScopedGlobalUnion,
		&parsed->fTemplates);

	rlc_scoped_union_create(
		RLC_BASE_CAST(this, RlcScopedUnion),
		file,
		parsed,
		RLC_BASE_CAST2(this, RlcScopedScopeEntry, RlcScopedScopeItem)->children);
}

void rlc_scoped_global_union_destroy(
	struct RlcScopedGlobalUnion * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
	rlc_scoped_union_destroy(
		RLC_BASE_CAST(this, RlcScopedUnion));
}

void rlc_scoped_member_union_create(
	struct RlcScopedMemberUnion * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberUnion const * parsed,
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
		kRlcScopedMemberUnion,
		&RLC_BASE_CAST(parsed, RlcParsedUnion)->fTemplates);

	rlc_scoped_union_create(
		RLC_BASE_CAST(this, RlcScopedUnion),
		file,
		RLC_BASE_CAST(parsed, RlcParsedUnion),
		RLC_BASE_CAST2(this, RlcScopedMember, RlcScopedScopeItem)->children);
}

void rlc_scoped_member_union_destroy(
	struct RlcScopedMemberUnion * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_union_destroy(
		RLC_BASE_CAST(this, RlcScopedUnion));

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));
}