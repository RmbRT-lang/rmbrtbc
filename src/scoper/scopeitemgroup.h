/** @file scopeitemgroup.h
	Contains the scope item group type. */
#ifndef __rlc_scoper_scopeitemgroup_h_defined
#define __rlc_scoper_scopeitemgroup_h_defined
#pragma once

#include "identifier.h"
#include "../src/string.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedScope;
struct RlcScopedScopeItem;

/** A group of scope items with the same name. */
struct RlcScopedScopeItemGroup
{
	/** The scope items' name. */
	struct RlcScopedIdentifier name;

	/** The items' parent scope. */
	struct RlcScopedScope * parent;

	/** The scope items.*/
	struct RlcScopedScopeItem ** items;
	/** The number of scope items. */
	RlcSrcSize itemCount;
};


/** Creates a scope item group.
@memberof RlcScopedScopeItemGroup
@param[out] this:
	The scope item group to create.
	@dassert @nonull
@param[in] name:
	The scope item group's name.
	@instance_ownership
	@dassert @nonnull
@param[in] parent:
	The scope this scope item group belongs to.
	@dassert @nonnull */
void rlc_scoped_scope_item_group_create(
	struct RlcScopedScopeItemGroup * this,
	struct RlcScopedIdentifier const * name,
	struct RlcScopedScope * parent);

/** Destroys a scope item group.
@memberof RlcScopedScopeItemGroup
@param[in,out] this:
	The scope item group to destroy.
	@dassert @nonnull */
void rlc_scoped_scope_item_group_destroy(
	struct RlcScopedScopeItemGroup * this);

/** Adds a scope item to a scope item group.
@memberof RlcScopedScopeItemGroup
@param[in,out] this:
	The scope item group to add an item to.
	@dassert @nonnull
@param[in] item:
	The item to add.
	@pointer_ownership */
void rlc_scoped_scope_item_group_add(
	struct RlcScopedScopeItemGroup * this,
	struct RlcScopedScopeItem * item);

#ifdef __cplusplus
}
#endif

#endif