/** @file scopeitem.h
	Contains the scope item type. */
#ifndef __rlc_scoper_scopeitem_h_defined
#define __rlc_scoper_scopeitem_h_defined
#pragma once

#include "identifier.h"
#include "templatedecl.h"

#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedScope;
struct RlcSrcFile;
struct RlcSrcString;

/** The deriving type of a scope item. */
enum RlcScopedScopeItemType
{
	/** RlcScopedScopeEntry. */
	kRlcScopedScopeEntry,
	/** RlcScopedMember. */
	kRlcScopedMember,

	RLC_ENUM_END(RlcScopedScopeItemType)
};

/** A scope's item.
	May either be a scope entry or a member. */
struct RlcScopedScopeItem
{
	RLC_ABSTRACT(RlcScopedScopeItem);

	/** The item's name group. */
	struct RlcScopedScopeItemGroup * group;
	/** The item's child items. */
	struct RlcScopedScope * children;
	/** The item's template declaration. */
	struct RlcScopedTemplateDecl templates;
};

/** Creates a scoped scope item.
@memberof RlcScopedScopeItem
@param[out] this:
	The scope item to create.
	@dassert @nonnull
@param[in] group:
	The item's name group.
	@dassert @nonnull
@param[in] makeChildren:
	Whether this scope item needs a scope.
	@dassert @nonnull
@param[in] type:
	The deriving type.
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] templates:
	The scope item's template declaration.
	@dassert @nonnull */
void rlc_scoped_scope_item_create(
	struct RlcScopedScopeItem * this,
	struct RlcScopedScopeItemGroup * group,
	int makeChildren,
	enum RlcScopedScopeItemType type,
	struct RlcSrcFile const * file,
	struct RlcParsedTemplateDecl const * templates);

void rlc_scoped_scope_item_destroy_base(
	struct RlcScopedScopeItem * this);

/** Destroys and deletes a scoped scope item.
@memberof RlcScopedScopeItem
@param[in,out] this:
	The scope item to delete.
	@dassert @nonnull */
void rlc_scoped_scope_item_delete(
	struct RlcScopedScopeItem * this);

#ifdef __cplusplus
}
#endif

#endif