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

void rlc_scoped_scope_entry_name_create(
	struct RlcScopedScopeEntryName * this,
	struct RlcSrcFile const * file,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(name != NULL);

	this->fName = rlc_src_string_cstr(name, file);
}

void rlc_scoped_scope_entry_name_destroy(
	struct RlcScopedScopeEntryName * this)
{
	RLC_DASSERT(this != NULL);
	rlc_free((void**)&this->fName);
}

int rlc_scoped_scope_entry_name_compare(
	struct RlcScopedScopeEntryName const * a,
	struct RlcScopedScopeEntryName const * b)
{
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);

	return strcmp(a->fName, b->fName);
}

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

	static struct {
		constructor_t constructor;
		intptr_t scopedOffset;
		intptr_t parsedOffset;
		enum RlcParsedScopeEntryType parsedType;
	} k_constructors[] = {
		ENTRY(rlc_scoped_enum_new, Enum),
		ENTRY(rlc_scoped_namespace_new, Namespace),
	};
#undef ENTRY

	/*static_assert(_countof(k_constructors) == RLC_COUNT(RlcParsedScopeEntryType),
		"Incomplete vtable."); //*/

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

	rlc_scoped_scope_entry_name_create(&this->name, file, name);
	this->file = file;
	this->parsed = parsed;
	this->references = 1;
	this->children = parent ? rlc_scoped_scope_new(parent) : NULL;
	RLC_DERIVING_TYPE(this) = type;
}


void rlc_scoped_scope_entry_destroy_base(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references == 0);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcScopedScopeEntryType));

	if(this->children)
		rlc_scoped_scope_delete(this->children);
	rlc_scoped_scope_entry_name_destroy(&this->name);
}

static void rlc_scoped_scope_entry_delete_virtual(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references == 0);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcScopedScopeEntryType));

	typedef void (*destructor_t)(
		void * this);
	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_scoped_namespace_delete,
		(destructor_t)&rlc_scoped_enum_delete,
		(destructor_t)&rlc_scoped_enum_constant_delete
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

void rlc_scoped_scope_entry_reference(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	++this->references;
}

void rlc_scoped_scope_entry_deref(
	struct RlcScopedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references != 0);

	if(!--this->references)
		rlc_scoped_scope_entry_delete_virtual(this);
}
