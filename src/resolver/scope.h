#ifndef __rlc_resolver_scope_h_defined
#define __rlc_resolver_scope_h_defined
#pragma once

#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A resolved scope.
	Scopes have a parent scope, into which they are embedded. Scopes can also have siblings (i.e., multiple namespaces with the same name, or the global namespace in multiple files), which are also included in the scope. */
struct RlcResolvedScope
{
	/** The parent scope, or NULL if global scope. */
	struct RlcResolvedScope * parent;

	/** The scope's siblings. */
	struct RlcResolvedScope ** siblings;
	/** The scope's sibling count. */
	RlcSrcSize siblingCount;

	/** The scope's entries. */
	struct RlcResolvedScopeEntry ** entries;
	/** The scope's entry count. */
	RlcSrcSize entryCount;
};

struct RlcResolvedScope * rlc_resolved_scope_new(
	struct RlcResolvedScope * parent);

void rlc_resolved_scope_delete(
	struct RlcResolvedScope * this);

/** A filter loop callback.
@return
	Whether to continue looking. */
typedef int (*rlc_resolved_scope_filter_fn_t)(
	struct RlcResolvedScopeEntry *,
	void *);
/** Calls a callback on all scope entries with the requested name.
	Also looks in all sibling and parent scopes if requested.
@memberof RlcResolvedScope
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
int rlc_resolved_scope_filter(
	struct RlcResolvedScope * this,
	struct RlcResolvedScopeEntryName const * name,
	rlc_resolved_scope_filter_fn_t callback,
	void * userdata,
	int check_parents,
	int check_siblings);

struct RlcResolvedScopeEntry * rlc_resolved_scope_add_entry(
	struct RlcResolvedScope * this,
	struct RlcSrcFile * file,
	struct RlcParsedScopeEntry * entry);

#ifdef __cplusplus
}
#endif


#endif