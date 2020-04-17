#include "scopeentry.h"
#include "class.h"
#include "scope.h"
#include "enum.h"
#include "function.h"
#include "rawtype.h"
#include "union.h"
#include "typedef.h"
#include "namespace.h"
#include "variable.h"
#include "externalsymbol.h"
#include "../parser/class.h"
#include "../parser/enum.h"
#include "../parser/function.h"
#include "../parser/namespace.h"
#include "../parser/rawtype.h"
#include "../parser/union.h"
#include "../parser/typedef.h"
#include "../parser/variable.h"
#include "../parser/externalsymbol.h"
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
		RLC_BASE_OFFSET(RlcScopedScopeEntry, struct RlcScoped##type), \
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsed##type), \
		kRlcParsed##type, \
		sizeof(struct RlcScoped##type) \
	}
#define GLOBAL_ENTRY(constructor, type) { \
		(constructor_t)&constructor, \
		RLC_BASE_OFFSET(RlcScopedScopeEntry, struct RlcScopedGlobal##type), \
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsed##type), \
		kRlcParsed##type, \
		sizeof(struct RlcScopedGlobal##type) \
	}
#define NOENTRY(type) { NULL, 0, 0, kRlcParsed##type, 0 }

	static struct {
		constructor_t constructor;
		intptr_t scopedOffset;
		intptr_t parsedOffset;
		enum RlcParsedScopeEntryType parsedType;
		size_t typeSize;
	} k_constructors[] = {
		GLOBAL_ENTRY(rlc_scoped_global_class_create, Class),
		GLOBAL_ENTRY(rlc_scoped_global_rawtype_create, Rawtype),
		GLOBAL_ENTRY(rlc_scoped_global_union_create, Union),
		ENTRY(rlc_scoped_namespace_create, Namespace),
		GLOBAL_ENTRY(rlc_scoped_global_function_create, Function),
		GLOBAL_ENTRY(rlc_scoped_global_variable_create, Variable),
		GLOBAL_ENTRY(rlc_scoped_global_enum_create, Enum),
		NOENTRY(EnumConstant), // Needs more context to be created.
		GLOBAL_ENTRY(rlc_scoped_global_typedef_create, Typedef),
		ENTRY(rlc_scoped_external_symbol_create, ExternalSymbol)
	};
#undef ENTRY
#undef GLOBAL_ENTRY
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

#define GLOBAL_ENTRY(destructor, type) { \
		(destructor_t)&destructor, \
		RLC_DERIVE_OFFSET(RlcScopedScopeEntry, struct RlcScopedGlobal##type), \
		kRlcParsed##type \
	}
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
		GLOBAL_ENTRY(rlc_scoped_global_class_destroy, Class),
		GLOBAL_ENTRY(rlc_scoped_global_rawtype_destroy, Rawtype),
		GLOBAL_ENTRY(rlc_scoped_global_union_destroy, Union),
		ENTRY(rlc_scoped_namespace_destroy, Namespace),
		GLOBAL_ENTRY(rlc_scoped_global_function_destroy, Function),
		GLOBAL_ENTRY(rlc_scoped_global_variable_destroy, Variable),
		GLOBAL_ENTRY(rlc_scoped_global_enum_destroy, Enum),
		ENTRY(rlc_scoped_enum_constant_destroy, EnumConstant),
		GLOBAL_ENTRY(rlc_scoped_global_typedef_destroy, Typedef),
		ENTRY(rlc_scoped_external_symbol_destroy, ExternalSymbol)
	};
#undef ENTRY
#undef GLOBAL_ENTRY
#undef NOENTRY

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedScopeEntryType), "ill-sized vtable.");

	enum RlcScopedScopeEntryType const type = RLC_DERIVING_TYPE(this);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedScopeEntryType));
	RLC_DASSERT(k_vtable[type].type == type);
	RLC_DASSERT(k_vtable[type].dtor);

	uint8_t * p = ((uint8_t*)this) + k_vtable[type].offset;
	k_vtable[type].dtor(p);

	rlc_free((void**)&p);
}