/** @file scopeitem.h
	Contains the resolved scope item type. */
#ifndef __rlc_resolver_scopeitem_h_defined
#define __rlc_resolver_scopeitem_h_defined

#include "../src/string.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedScope;
struct RlcScopedSymbol;
struct RlcResolvedTemplateArgs;

/** A resolved scope item reference. */
struct RlcResolvedScopeItem
{
	/** All the template arguments in the scope path. */
	struct RlcResolvedTemplateArgs * templates;
	/** The scope path depth. */
	RlcSrcSize templateCount;
	/** The scope item. */
	struct RlcScopedScopeItem * scopeItem;
};

void rlc_resolved_scope_item_create(
	struct RlcResolvedScopeItem * this,
	struct RlcScopedSymbol * symbol,
	struct RlcScopedScope * scope);

void rlc_resolved_scope_item_destroy(
	struct RlcResolvedScopeItem * this);

#ifdef __cplusplus
}
#endif

#endif