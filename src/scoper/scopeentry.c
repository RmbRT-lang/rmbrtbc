#include "scopeentry.h"
#include "scope.h"
#include "enum.h"
#include "namespace.h"
#include "../parser/enum.h"
#include "../parser/namespace.h"
#include "../assert.h"
#include "../malloc.h"

#include "../parser/member.h"

#include <string.h>

struct RlcScopedScopeEntry * rlc_scoped_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(parsed), RlcParsedScopeEntryType));

	typedef uint8_t * (*constructor_t)(
		struct RlcSrcFile const *,
		void const *,
		struct RlcScopedScope *);

#define ENTRY(constructor, type) { \
		(constructor_t)&constructor, \
		RLC_DERIVE_OFFSET(RlcScopedScopeEntry, struct RlcScoped##type), \
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsed##type), \
		kRlcParsed##type \
	}
#define NOENTRY(type) { NULL, 0, 0, kRlcParsed##type }

	static struct {
		constructor_t constructor;
		intptr_t scopedOffset;
		intptr_t parsedOffset;
		enum RlcParsedScopeEntryType parsedType;
	} k_constructors[] = {
		ENTRY(rlc_scoped_enum_new, Enum),
		NOENTRY(EnumConstant),
		ENTRY(rlc_scoped_namespace_new, Namespace),
	};
#undef ENTRY
#undef NOENTRY

	/*static_assert(RLC_COVERS_ENUM(k_constructors, RlcParsedScopeEntryType),
		"Incomplete vtable."); // */
	RLC_DASSERT(k_constructors[RLC_DERIVING_TYPE(parsed)].constructor);

	uint8_t * ret;
	ret = k_constructors[RLC_DERIVING_TYPE(parsed)].constructor(
		file,
		(uint8_t *)parsed + k_constructors[RLC_DERIVING_TYPE(parsed)].parsedOffset,
		parent);
	ret += k_constructors[RLC_DERIVING_TYPE(parsed)].scopedOffset;

	return (struct RlcScopedScopeEntry *) ret;
}

void rlc_scoped_scope_entry_create(
	struct RlcScopedScopeEntry * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed,
	struct RlcScopedScope * parent,
	enum RlcScopedScopeEntryType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedScopeEntryType));

	rlc_scoped_scope_entry_create_custom_name(
		this,
		file,
		parsed,
		parent,
		&parsed->fName,
		type);
}

void rlc_scoped_scope_entry_create_custom_name(
	struct RlcScopedScopeEntry * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed,
	struct RlcScopedScope * parent,
	struct RlcSrcString const * name,
	enum RlcScopedScopeEntryType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedScopeEntryType));

	struct RlcScopedIdentifier ident;
	rlc_scoped_identifier_create(&ident, file, name);
	rlc_scoped_scope_item_create(
		RLC_BASE_CAST(this, RlcScopedScopeItem),
		&ident,
		parent,
		1,
		kRlcScopedScopeEntry);

	this->file = file;
	this->parsed = parsed;
	RLC_DERIVING_TYPE(this) = type;
}


void rlc_scoped_scope_entry_destroy_base(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcScopedScopeEntryType));

	rlc_scoped_scope_item_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeItem));
}

void rlc_scoped_scope_entry_destroy_virtual(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcScopedScopeEntryType));

	typedef void (*destructor_t)(
		void * this);
	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_scoped_namespace_destroy,
		(destructor_t)&rlc_scoped_enum_destroy,
		(destructor_t)&rlc_scoped_enum_constant_destroy
	};
	(void) k_vtable;

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedScopeEntryType), "ill-sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcScopedScopeEntry, struct RlcScopedNamespace),
		RLC_DERIVE_OFFSET(RlcScopedScopeEntry, struct RlcScopedEnum),
		RLC_DERIVE_OFFSET(RlcScopedScopeEntry, struct RlcScopedEnumConstant)
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcScopedScopeEntryType), "ill-sized offset table.");

	k_vtable[RLC_DERIVING_TYPE(this)]((uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)]);
}