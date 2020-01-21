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
	struct RlcSrcString const * name);
void rlc_scoped_scope_entry_name_destroy(
	struct RlcScopedScopeEntryName * this);

int rlc_scoped_scope_entry_name_compare(
	struct RlcScopedScopeEntryName const * a,
	struct RlcScopedScopeEntryName const * b);

enum RlcScopedScopeEntryType
{
	kRlcScopedNamespace,
	kRlcScopedEnum,
	kRlcScopedEnumConstant,
	RLC_ENUM_END(RlcScopedScopeEntryType)
};

/** An entry within a scoped scope.
	This type is a shared resource. */
struct RlcScopedScopeEntry
{
	RLC_ABSTRACT(RlcScopedScopeEntry);
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
	References the scoped scope entry once.
@memberof RlcScopedScopeEntry
@param[in] file:
	The file in which the scope entry resides.
	@dassert @nonnull
@param[in] parsed:
	The parsed entity.
	@dassert @nonnull
@param[in] parent:
	The parent scope.
	@dassert @nonnull
@return
	The created scope entry. */
struct RlcScopedScopeEntry * rlc_scoped_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * parsed,
	struct RlcScopedScope * parent);

/** Creates a scoped scope entry from a parsed entity.
	References the scoped scope entry once.
@memberof RlcScopedScopeEntry
@param[out] this:
	The scoped scope entry to create.
	@dassert @nonnull
@param[in] file:
	The file in which the scope entry resides.
	@dassert @nonnull
@param[in] parsed:
	The parsed entity.
	@dassert @nonnull
@param[in] parent:
	If this scope entries has children, then this points to the parent scope, or null, otherwise.
@param[in] type:
	The deriving type. */
void rlc_scoped_scope_entry_create(
	struct RlcScopedScopeEntry * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed,
	struct RlcScopedScope * parent,
	enum RlcScopedScopeEntryType type);

/** Creates a scoped scope entry from a parsed entity using a custom name.
	References the scoped scope entry once.
@memberof RlcScopedScopeEntry
@param[out] this:
	The scoped scope entry to create.
	@dassert @nonnull
@param[in] file:
	The file in which the scope entry resides.
	@dassert @nonnull
@param[in] parsed:
	The parsed entity.
	@dassert @nonnull
@param[in] parent:
	If this scope entries has children, then this points to the parent scope, or null, otherwise.
@param[in] name:
	The name to use for the scope entry.
	@dassert @nonnull
@param[in] type:
	The deriving type. */
void rlc_scoped_scope_entry_create_custom_name(
	struct RlcScopedScopeEntry * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed,
	struct RlcScopedScope * parent,
	struct RlcSrcString const * name,
	enum RlcScopedScopeEntryType type);

/** Destroys the base of a scoped scope entry.
@memberof RlcScopedScopeEntry
@param[in,out] this:
	The scope entry to destroy.
	@dassert @nonnull */
void rlc_scoped_scope_entry_destroy_base(
	struct RlcScopedScopeEntry * this);

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