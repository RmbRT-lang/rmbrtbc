#include "rawtype.h"
#include "expression.h"
#include "member.h"
#include "scope.h"
#include "../parser/rawtype.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedRawtype kRlcParsedRawtype

struct RlcScopedRawtype * rlc_scoped_rawtype_new(
	struct RlcSrcFile const * file,
	struct RlcParsedRawtype const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	struct RlcScopedRawtype * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedRawtype));

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(ret, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedRawtype);

	ret->fSize = rlc_scoped_expression_new(parsed->fSize, file);
	struct RlcScopedScopeItem * retAsItem = RLC_BASE_CAST(
		RLC_BASE_CAST(ret, RlcScopedScopeEntry),
		RlcScopedScopeItem);

	for(RlcSrcIndex i = 0; i < parsed->fMembers.fEntryCount; i++)
	{
		rlc_scoped_scope_add_member(
			retAsItem->children,
			file,
			parsed->fMembers.fEntries[i]);
	}

	return ret;
}

void rlc_scoped_rawtype_delete(
	struct RlcScopedRawtype * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_scoped_expression_delete_virtual(this->fSize);

	rlc_free((void**)&this);
}