#include "enum.h"
#include "scope.h"
#include "scopeitemgroup.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedEnum kRlcParsedEnum
#define kRlcScopedEnumConstant kRlcParsedEnumConstant

void rlc_scoped_enum_create(
	struct RlcScopedEnum * this,
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
				RLC_BASE_CAST2(this, RlcScopedScopeEntry, RlcScopedScopeItem)->children,
				&nameToken,
				file);

			struct RlcScopedEnumConstant * constant =
				rlc_scoped_enum_constant_new(
					file,
					&parsed->fConstants[i],
					i,
					group);

			rlc_scoped_scope_item_group_add(
				group,
				RLC_BASE_CAST2(
					constant,
					RlcScopedScopeEntry,
					RlcScopedScopeItem));
		}
}

void rlc_scoped_enum_destroy(
	struct RlcScopedEnum * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}

struct RlcScopedEnumConstant * rlc_scoped_enum_constant_new(
	struct RlcSrcFile const * file,
	struct RlcParsedEnumConstant const * parsed,
	RlcSrcIndex value,
	struct RlcScopedScopeItemGroup * parent)
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
		parent,
		kRlcScopedEnumConstant,
		NULL);

	RLC_DASSERT(parent->parent->ownerIsItem);
	ret->fValue = value;
	ret->fType = RLC_DERIVE_CAST(
		RLC_DERIVE_CAST(
			parent->parent->ownerItem,
			RlcScopedScopeItem,
			struct RlcScopedScopeEntry),
		RlcScopedScopeEntry,
		struct RlcScopedEnum);

	return ret;
}

void rlc_scoped_enum_constant_destroy(
	struct RlcScopedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}