#include "scopeitem.h"
#include "scopeitemgroup.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_scoped_scope_item_group_create(
	struct RlcScopedScopeItemGroup * this,
	struct RlcScopedIdentifier const * name,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(parent != NULL);

	this->name = *name;
	this->parent = parent;
	this->items = NULL;
	this->itemCount = 0;
}

void rlc_scoped_scope_item_group_destroy(
	struct RlcScopedScopeItemGroup * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_identifier_destroy(&this->name);

	if(this->items)
	{
		for(RlcSrcIndex i = 0; i < this->itemCount; i++)
			rlc_scoped_scope_item_delete(this->items[i]);

		rlc_free((void**)&this->items);
		this->itemCount = 0;
	}
}

void rlc_scoped_scope_item_group_add(
	struct RlcScopedScopeItemGroup * this,
	struct RlcScopedScopeItem * item)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(item != NULL);

	rlc_realloc(
		(void**)&this->items,
		++this->itemCount * sizeof(struct RlcScopedScopeItem *));

	this->items[this->itemCount-1] = item;
}