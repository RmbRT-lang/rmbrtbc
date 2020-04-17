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
	enum RlcScopedScopeItemType type,
	struct RlcSrcFile const * file,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(group != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(templates != NULL);

	RLC_DERIVING_TYPE(this) = type;
	this->group = group;
	if(makeChildren)
		this->children = rlc_scoped_scope_new_for_item(this);

	rlc_scoped_template_decl_create(&this->templates, file, templates);
}

void rlc_scoped_scope_item_destroy_base(
	struct RlcScopedScopeItem * this)
{
	RLC_DASSERT(this != NULL);

	if(this->children)
		rlc_scoped_scope_delete(this->children);

	rlc_scoped_template_decl_destroy(&this->templates);
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

	char * p = ((char *) this) + k_vtable[RLC_DERIVING_TYPE(this)].offset;
	k_vtable[RLC_DERIVING_TYPE(this)].dtor(p);
}