#include "enum.h"
#include "scope.h"
#include "scopeitemgroup.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedEnum kRlcParsedEnum
#define kRlcScopedMemberEnum kRlcParsedMemberEnum
#define kRlcScopedEnumConstant kRlcParsedEnumConstant

static void rlc_scoped_enum_create(
	struct RlcScopedEnum * this,
	struct RlcSrcFile const * file,
	struct RlcParsedEnum const * parsed,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->fSize = parsed->fConstantCount;

	for(RlcSrcIndex i = 0; i < parsed->fConstantCount; i++)
		for(RlcSrcIndex j = 0; j <= parsed->fConstants[i].fAliasCount; j++)
		{
			struct RlcToken nameToken;
			nameToken.type = kRlcTokIdentifier;
			if(j == 0)
				nameToken.content = RLC_BASE_CAST(
					&parsed->fConstants[i],
					RlcParsedScopeEntry)->fName;
			else
				nameToken.content = parsed->fConstants[i].fAliasTokens[j];

			struct RlcScopedScopeItemGroup * group = rlc_scoped_scope_group(
				scope,
				&nameToken,
				file);

			struct RlcScopedEnumConstant * constant =
				rlc_scoped_enum_constant_new(
					file,
					&parsed->fConstants[i],
					i,
					group,
					this);

			rlc_scoped_scope_item_group_add(
				group,
				RLC_BASE_CAST2(
					constant,
					RlcScopedScopeEntry,
					RlcScopedScopeItem));
		}
}

static void rlc_scoped_enum_destroy(
	struct RlcScopedEnum * this)
{
	RLC_DASSERT(this != NULL);

	(void) this;
}

void rlc_scoped_global_enum_create(
	struct RlcScopedGlobalEnum * this,
	struct RlcSrcFile const * file,
	struct RlcParsedEnum const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedEnum,
		NULL);

	rlc_scoped_enum_create(
		RLC_BASE_CAST(this, RlcScopedEnum),
		file,
		parsed,
		RLC_BASE_CAST2(
			this,
			RlcScopedScopeEntry,
			RlcScopedScopeItem)->children);
}

void rlc_scoped_global_enum_destroy(
	struct RlcScopedGlobalEnum * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_scoped_enum_destroy(
		RLC_BASE_CAST(this, RlcScopedEnum));
}

void rlc_scoped_member_enum_create(
	struct RlcScopedMemberEnum * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberEnum * parsed,
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
		kRlcScopedMemberEnum,
		NULL);

	rlc_scoped_enum_create(
		RLC_BASE_CAST(this, RlcScopedEnum),
		file,
		RLC_BASE_CAST(parsed, RlcParsedEnum),
		RLC_BASE_CAST2(this, RlcScopedMember, RlcScopedScopeItem)->children);

}

void rlc_scoped_member_enum_destroy(
	struct RlcScopedMemberEnum * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));

	rlc_scoped_enum_destroy(
		RLC_BASE_CAST(this, RlcScopedEnum));
}

struct RlcScopedEnumConstant * rlc_scoped_enum_constant_new(
	struct RlcSrcFile const * file,
	struct RlcParsedEnumConstant const * parsed,
	RlcSrcIndex value,
	struct RlcScopedScopeItemGroup * group,
	struct RlcScopedEnum * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	struct RlcScopedEnumConstant * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedEnumConstant));

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(ret, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		group,
		kRlcScopedEnumConstant,
		NULL);

	ret->fValue = value;
	ret->fType = parent;

	return ret;
}

void rlc_scoped_enum_constant_destroy(
	struct RlcScopedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}