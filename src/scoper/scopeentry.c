#include "scopeentry.h"
#include "class.h"
#include "scope.h"
#include "enum.h"
#include "function.h"
#include "rawtype.h"
#include "typedef.h"
#include "namespace.h"
#include "variable.h"
#include "../parser/class.h"
#include "../parser/enum.h"
#include "../parser/function.h"
#include "../parser/namespace.h"
#include "../parser/rawtype.h"
#include "../parser/typedef.h"
#include "../parser/variable.h"
#include "../assert.h"
#include "../malloc.h"

#include "../parser/member.h"

#include <string.h>

struct RlcScopedScopeEntry * rlc_scoped_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(parsed), RlcParsedScopeEntryType));

	typedef void (*constructor_t)(
		uint8_t *,
		struct RlcSrcFile const *,
		void const *,
		struct RlcScopedScopeItemGroup *);

#define ENTRY(constructor, type) { \
		(constructor_t)&constructor, \
		RLC_DERIVE_OFFSET(RlcScopedScopeEntry, struct RlcScoped##type), \
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsed##type), \
		kRlcParsed##type, \
		sizeof(struct RlcScoped##type) \
	}
#define NOENTRY(type) { NULL, 0, 0, kRlcParsed##type, 0 }

	static struct {
		constructor_t constructor;
		intptr_t scopedOffset;
		intptr_t parsedOffset;
		enum RlcParsedScopeEntryType parsedType;
		size_t typeSize;
	} k_constructors[] = {
		ENTRY(rlc_scoped_class_create, Class),
		ENTRY(rlc_scoped_rawtype_create, Rawtype),
		NOENTRY(Union),
		ENTRY(rlc_scoped_namespace_create, Namespace),
		ENTRY(rlc_scoped_function_create, Function),
		ENTRY(rlc_scoped_variable_create, Variable),
		ENTRY(rlc_scoped_enum_create, Enum),
		NOENTRY(EnumConstant), // Needs more context to be created.
		ENTRY(rlc_scoped_typedef_create, Typedef),
		NOENTRY(ExternalSymbol)
	};
#undef ENTRY
#undef NOENTRY

	enum RlcParsedScopeEntryType const type = RLC_DERIVING_TYPE(parsed);

	static_assert(RLC_COVERS_ENUM(k_constructors, RlcParsedScopeEntryType),
		"Incomplete vtable.");
	RLC_DASSERT(k_constructors[type].constructor);
	RLC_DASSERT(k_constructors[type].parsedType == type);

	uint8_t * ret = NULL;
	rlc_malloc((void**)&ret, k_constructors[type].typeSize);
	k_constructors[type].constructor(
		ret,
		file,
		(uint8_t *)parsed + k_constructors[type].parsedOffset,
		parent);
	ret += k_constructors[type].scopedOffset;

	return (struct RlcScopedScopeEntry *) ret;
}

void rlc_scoped_scope_entry_create(
	struct RlcScopedScopeEntry * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed,
	struct RlcScopedScopeItemGroup * parent,
	enum RlcScopedScopeEntryType type,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedScopeEntryType));

	rlc_scoped_scope_item_create(
		RLC_BASE_CAST(this, RlcScopedScopeItem),
		parent,
		1,
		kRlcScopedScopeEntry,
		file,
		templates ? templates : &kRlcParsedTemplateDeclNone);

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

#define ENTRY(destructor, type) { \
		(destructor_t)&destructor, \
		RLC_DERIVE_OFFSET(RlcScopedScopeEntry, struct RlcScoped##type), \
		kRlcParsed##type \
	}
#define NOENTRY(type) { NULL, 0, kRlcParsed##type }
	static struct {
		destructor_t dtor;
		intptr_t offset;
		enum RlcScopedScopeEntryType type;
	} const k_vtable[] = {
		ENTRY(rlc_scoped_class_destroy, Class),
		ENTRY(rlc_scoped_rawtype_destroy, Rawtype),
		NOENTRY(Union),
		ENTRY(rlc_scoped_namespace_destroy, Namespace),
		ENTRY(rlc_scoped_function_destroy, Function),
		ENTRY(rlc_scoped_variable_destroy, Variable),
		ENTRY(rlc_scoped_enum_destroy, Enum),
		ENTRY(rlc_scoped_enum_constant_destroy, EnumConstant),
		ENTRY(rlc_scoped_typedef_destroy, Typedef),
		NOENTRY(ExternalSymbol)
	};
#undef ENTRY
#undef NOENTRY

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedScopeEntryType), "ill-sized vtable.");

	enum RlcScopedScopeEntryType const type = RLC_DERIVING_TYPE(this);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedScopeEntryType));
	RLC_DASSERT(k_vtable[type].type == type);
	RLC_DASSERT(k_vtable[type].dtor);

	k_vtable[type].dtor(((uint8_t*)this) + k_vtable[type].offset);
}