#ifndef __rlc_scoper_scopeentry_h_defined
#define __rlc_scoper_scopeentry_h_defined
#pragma once

#include "../parser/scopeentry.h"
#include "symbol.h"

#include "scopeitem.h"

#ifdef __cplusplus
extern "C" {
#endif

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
	RLC_DERIVE(struct,RlcScopedScopeItem);

	/** The file in which the scope entry resides. */
	struct RlcSrcFile const * file;
	/** The scoped scope entry's parsed representation. */
	struct RlcParsedScopeEntry const * parsed;
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
	struct RlcScopedScopeItemGroup * parent);

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
	The deriving type.
@param[in] templates:
	The member's template declaration, or null. */
void rlc_scoped_scope_entry_create(
	struct RlcScopedScopeEntry * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed,
	struct RlcScopedScopeItemGroup * parent,
	enum RlcScopedScopeEntryType type,
	struct RlcParsedTemplateDecl const * templates);

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

/** Destroys a scoped scope entry.
@memberof RlcScopedScopeEntry
@param[in,out] this:
	The scope entry to destroy.
	@dassert @nonnull */
void rlc_scoped_scope_entry_destroy_virtual(
	struct RlcScopedScopeEntry * this);

#ifdef __cplusplus
}
#endif

#endif