#include "namespace.h"
#include "scope.h"
#include "../malloc.h"

struct RlcScopedNamespace * rlc_scoped_namespace_new(
	struct RlcSrcFile const * file,
	struct RlcParsedNamespace const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	struct RlcScopedNamespace * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedNamespace));

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(ret, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedNamespace);

	for(RlcSrcIndex i = 0; i < parsed->fEntryList.fEntryCount; i++)
		rlc_scoped_scope_add_entry(
			RLC_BASE_CAST(
				RLC_BASE_CAST(ret, RlcScopedScopeEntry),
				RlcScopedScopeItem)->children,
			file,
			parsed->fEntryList.fEntries[i]);

	return ret;
}

void rlc_scoped_namespace_destroy(
	struct RlcScopedNamespace * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}