#include "enum.h"
#include "scope.h"
#include "../assert.h"
#include "../malloc.h"

struct RlcScopedEnum * rlc_scoped_enum_new(
	struct RlcSrcFile const * file,
	struct RlcParsedEnum const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	struct RlcScopedEnum * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcParsedEnum));

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(ret, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedEnum);

	ret->fSize = parsed->fConstantCount;

	for(RlcSrcIndex i = 0; i < parsed->fConstantCount; i++)
		for(RlcSrcIndex j = 0; j <= parsed->fConstants[i].fAliasCount; j++)
		{
			struct RlcScopedEnumConstant * constant =
				rlc_scoped_enum_constant_new(
					file,
					&parsed->fConstants[i],
					j,
					i,
					ret);
			rlc_scoped_scope_add_entry_custom(
				RLC_BASE_CAST(ret, RlcScopedScopeEntry)->children,
				RLC_BASE_CAST(constant, RlcScopedScopeEntry));
		}

	return ret;
}

void rlc_scoped_enum_delete(
	struct RlcScopedEnum * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_free((void**)&this);
}

struct RlcScopedEnumConstant * rlc_scoped_enum_constant_new(
	struct RlcSrcFile const * file,
	struct RlcParsedEnumConstant const * parsed,
	RlcSrcIndex nameIndex,
	RlcSrcIndex value,
	struct RlcScopedEnum * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);
	RLC_DASSERT(nameIndex <= parsed->fAliasCount);

	struct RlcScopedEnumConstant * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedEnumConstant));

	struct RlcSrcString const * name;
	if(nameIndex == 0)
		name = &RLC_BASE_CAST(parsed, RlcParsedScopeEntry)->fName;
	else
		name = &parsed->fAliasTokens[nameIndex-1];

	rlc_scoped_scope_entry_create_custom_name(
		RLC_BASE_CAST(ret, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		RLC_BASE_CAST(parent, RlcScopedScopeEntry)->children,
		name,
		kRlcScopedEnumConstant);

	ret->fValue = value;
	ret->fType = parent;

	return ret;
}

void rlc_scoped_enum_constant_delete(
	struct RlcScopedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_free((void**)&this);
}