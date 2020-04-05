#include "namespace.h"
#include "scope.h"
#include "../malloc.h"

#define kRlcScopedNamespace kRlcParsedNamespace

void rlc_scoped_namespace_create(
	struct RlcScopedNamespace * this,
	struct RlcSrcFile const * file,
	struct RlcParsedNamespace const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedNamespace,
		NULL);

	for(RlcSrcIndex i = 0; i < parsed->fEntryList.fEntryCount; i++)
		rlc_scoped_scope_add_entry(
			RLC_BASE_CAST(
				RLC_BASE_CAST(this, RlcScopedScopeEntry),
				RlcScopedScopeItem)->children,
			file,
			parsed->fEntryList.fEntries[i]);
}

void rlc_scoped_namespace_destroy(
	struct RlcScopedNamespace * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}