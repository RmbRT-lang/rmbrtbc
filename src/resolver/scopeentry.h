#ifndef __rlc_resolver_scopeentry_h_defined
#define __rlc_resolver_scopeentry_h_defined
#pragma once

#include "../parser/scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A resolved scope entry's name's type. */
enum RlcResolvedScopeEntryNameType
{
	kRlcResolvedScopeEntryNameTypeIdentifier,
	kRlcResolvedScopeEntryNameTypeConstructor,
	kRlcResolvedScopeEntryNameTypeDestructor
};

/** A resolved scope entry's name. */
const struct RlcResolvedScopeEntryName
{
	enum RlcResolvedScopeEntryNameType type;
	char const * name;
}	kRlcResolvedScopeEntryNameConstructor,
	kRlcResolvedScopeEntryNameDestructor;

/** An entry within a resolved scope.
	This type is a shared resource. */
struct RlcResolvedScopeEntry
{
	/** The scope entry's name. */
	struct RlcResolvedScopeEntryName name;
	/** The file in which the scope entry resides. */
	struct RlcSrcFile const * file;
	/** The resolved scope entry's parsed representation. */
	struct RlcParsedScopeEntry * parsed;
	/** The entry's reference count. */
	size_t references;
};

/** Creates a resolved scope entry from a parsed entity.
@memberof RlcResolvedScopeEntry
@param[in] file:
	The file in which the scope entry resides.
	@dassert @nonnull
@param[in] parsed:
	The parsed entity.
	@dassert @nonnull
@return
	The resolved scope entry. It is referenced once already. */
struct RlcResolvedScopeEntry * rlc_resolved_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * parsed);
/** Increases a scope entry's reference count.
@memberof RlcResolvedScopeEntry
@param[in,out] this:
	The scope entry to reference.
	@dassert @nonnull */
void rlc_resolved_scope_entry_reference(
	struct RlcResolvedScopeEntry * this);
/** Decreases a scope entry's reference count.
	If the reference count reaches 0, deletes the scope entry and its reference to the parsed entity.
@memberof RlcResolvedScopeEntry
@param[in,out] this:
	The scope entry to reference.
	@dassert @nonnull */
void rlc_resolved_scope_entry_deref(
	struct RlcResolvedScopeEntry * this);

void rlc_resolved_scope_entry_name_create(
	struct RlcResolvedScopeEntryName * this,
	struct RlcSrcFile * file,
	struct RlcParsedScopeEntry * parsed);

void rlc_resolved_scope_entry_name_destroy(
	struct RlcResolvedScopeEntryName * this);

int rlc_resolved_scope_entry_name_compare(
	struct RlcResolvedScopeEntryName const * a,
	struct RlcResolvedScopeEntryName const * b);

#ifdef __cplusplus
}
#endif

#endif