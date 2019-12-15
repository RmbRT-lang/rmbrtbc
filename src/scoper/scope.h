#ifndef __rlc_scoper_scope_h_defined
#define __rlc_scoper_scope_h_defined
#pragma once

#include "scopeentry.h"
#include "../parser/file.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A scoped scope.
	Scopes have a parent scope, into which they are embedded. Scopes can also have siblings (i.e., multiple namespaces with the same name, or the global namespace in multiple files), which are also included in the scope. */
struct RlcScopedScope
{
	/** The parent scope, or NULL if global scope. */
	struct RlcScopedScope * parent;

	/** The scope's siblings. */
	struct RlcScopedScope ** siblings;
	/** The scope's sibling count. */
	RlcSrcSize siblingCount;

	/** The scope's entries. */
	struct RlcScopedScopeEntry ** entries;
	/** The scope's entry count. */
	RlcSrcSize entryCount;
};

struct RlcScopedScope * rlc_scoped_scope_new(
	struct RlcScopedScope * parent);

void rlc_scoped_scope_delete(
	struct RlcScopedScope * this);

/** A filter loop callback.
@return
	Whether to continue looking. */
typedef int (*rlc_scoped_scope_filter_fn_t)(
	struct RlcScopedScopeEntry *,
	void *);
/** Calls a callback on all scope entries with the requested name.
	Also looks in all sibling and parent scopes if requested.
@memberof RlcScopedScope
@param[in,out] this:
	The scope to search.
	@dassert @nonnull
@param[in] name:
	The name to look for.
@param[in] callback:
	The callback to execute on any found entries.
@param[in,out] userdata:
	Additional data to pass to the callback.
@param[in] check_parents:
	Whether to check parent scopes.
@param[in] check_siblings:
	Whether to check sibling scopes.
@return
	Whether any scope entries were found. */
int rlc_scoped_scope_filter(
	struct RlcScopedScope * this,
	struct RlcScopedScopeEntryName const * name,
	rlc_scoped_scope_filter_fn_t callback,
	void * userdata,
	int check_parents,
	int check_siblings);

struct RlcScopedScopeEntry * rlc_scoped_scope_add_entry(
	struct RlcScopedScope * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * entry);

void rlc_scoped_scope_populate(
	struct RlcScopedScope * this,
	struct RlcParsedFile const * file);

#ifdef __cplusplus
}
#endif


#endif