#include "scopeentry.h"
#include "../assert.h"
#include "../malloc.h"

#include "../parser/member.h"


struct RlcScopedScopeEntry * rlc_scoped_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * parsed)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	struct RlcScopedScopeEntry * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedScopeEntry));
	ret->file = file;
	ret->parsed = parsed;
	ret->references = 1;

	return ret;
}

static void rlc_scoped_scope_entry_destroy(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references == 0);

	rlc_scoped_symbol_child_destroy(&this->name);
	rlc_free((void**)&this->parsed);
	rlc_free((void**)&this);
}

void rlc_scoped_scope_entry_reference(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	++this->references;
}

void rlc_scoped_scope_entry_deref(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references != 0);

	if(--this->references)
		rlc_scoped_scope_entry_destroy(this);
}
