#include "scopeitem.h"
#include "scope.h"
#include "scopeentry.h"
#include "member.h"
#include "../src/string.h"
#include "../malloc.h"

#include <string.h>

void rlc_scoped_scope_item_create(
	struct RlcScopedScopeItem * this,
	struct RlcScopedScopeItemGroup * group,
	int makeChildren,
	enum RlcScopedScopeItemType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(group != NULL);

	RLC_DERIVING_TYPE(this) = type;
	this->group = group;
	if(makeChildren)
		this->children = rlc_scoped_scope_new(this);
}

void rlc_scoped_scope_item_destroy_base(
	struct RlcScopedScopeItem * this)
{
	RLC_DASSERT(this != NULL);

	if(this->children)
		rlc_scoped_scope_delete(this->children);
}

void rlc_scoped_scope_item_delete(
	struct RlcScopedScopeItem * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcScopedScopeItemType));

	typedef void (*destructor_t)(void *);

#define ENTRY(dtor, type) { \
		(destructor_t) &dtor, \
		RLC_DERIVE_OFFSET(RlcScopedScopeItem, struct type), \
		k##type \
	}
	static struct {
		destructor_t dtor;
		intptr_t offset;
		enum RlcScopedScopeItemType type;
	} const k_vtable[] = {
		ENTRY(rlc_scoped_scope_entry_destroy_virtual, RlcScopedScopeEntry),
		ENTRY(rlc_scoped_member_destroy_virtual, RlcScopedMember)
	};
#undef ENTRY
	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedScopeItemType),
		"ill-sized vtable");

	RLC_DASSERT(k_vtable[RLC_DERIVING_TYPE(this)].type == RLC_DERIVING_TYPE(this));
	k_vtable[RLC_DERIVING_TYPE(this)].dtor(
		((char *) this) + k_vtable[RLC_DERIVING_TYPE(this)].offset);
	rlc_free((void**)&this);
}