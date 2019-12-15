#include "scopeentry.h"
#include "scope.h"
#include "../assert.h"
#include "../malloc.h"

#include "../parser/member.h"

#include <string.h>

void rlc_scoped_scope_entry_name_create(
	struct RlcScopedScopeEntryName * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->fName = rlc_src_string_cstr(&parsed->fName, file);
}

void rlc_scoped_scope_entry_name_destroy(
	struct RlcScopedScopeEntryName * this)
{
	RLC_DASSERT(this != NULL);
	rlc_free((void**)&this->fName);
}

int rlc_scoped_scope_entry_name_compare(
	struct RlcScopedScopeEntryName const * a,
	struct RlcScopedScopeEntryName const * b)
{
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);

	return strcmp(a->fName, b->fName);
}

struct RlcScopedScopeEntry * rlc_scoped_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * parsed)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	struct RlcScopedScopeEntry * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedScopeEntry));
	rlc_scoped_scope_entry_name_create(&ret->name, file, parsed);
	ret->file = file;
	ret->parsed = parsed;
	ret->references = 1;
	ret->children = NULL;

	return ret;
}

static void rlc_scoped_scope_entry_destroy(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references == 0);

	if(this->children)
		rlc_scoped_scope_delete(this->children);
	rlc_scoped_scope_entry_name_destroy(&this->name);
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

	if(!--this->references)
		rlc_scoped_scope_entry_destroy(this);
}
