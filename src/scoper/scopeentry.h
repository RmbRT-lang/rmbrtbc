#ifndef __rlc_scoper_scopeentry_h_defined
#define __rlc_scoper_scopeentry_h_defined
#pragma once

#include "../parser/scopeentry.h"
#include "symbol.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A scope entry name. */
struct RlcScopedScopeEntryName
{
	char const * fName;
};

void rlc_scoped_scope_entry_name_create(
	struct RlcScopedScopeEntryName * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed);
void rlc_scoped_scope_entry_name_destroy(
	struct RlcScopedScopeEntryName * this);

int rlc_scoped_scope_entry_name_compare(
	struct RlcScopedScopeEntryName const * a,
	struct RlcScopedScopeEntryName const * b);


/** An entry within a scoped scope.
	This type is a shared resource. */
struct RlcScopedScopeEntry
{
	/** The scope entry's name. */
	struct RlcScopedScopeEntryName name;
	/** The file in which the scope entry resides. */
	struct RlcSrcFile const * file;
	/** The scoped scope entry's parsed representation. */
	struct RlcParsedScopeEntry const * parsed;
	/** The entry's reference count. */
	size_t references;
	/** The entry's child entries. */
	struct RlcScopedScope * children;
};

/** Creates a scoped scope entry from a parsed entity.
@memberof RlcScopedScopeEntry
@param[in] file:
	The file in which the scope entry resides.
	@dassert @nonnull
@param[in] parsed:
	The parsed entity.
	@dassert @nonnull
@return
	The scoped scope entry. It is referenced once already. */
struct RlcScopedScopeEntry * rlc_scoped_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * parsed);
/** Increases a scope entry's reference count.
@memberof RlcScopedScopeEntry
@param[in,out] this:
	The scope entry to reference.
	@dassert @nonnull */
void rlc_scoped_scope_entry_reference(
	struct RlcScopedScopeEntry * this);
/** Decreases a scope entry's reference count.
	If the reference count reaches 0, deletes the scope entry and its reference to the parsed entity.
@memberof RlcScopedScopeEntry
@param[in,out] this:
	The scope entry to reference.
	@dassert @nonnull */
void rlc_scoped_scope_entry_deref(
	struct RlcScopedScopeEntry * this);

#ifdef __cplusplus
}
#endif

#endif