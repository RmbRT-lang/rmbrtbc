#include "scopeentry.h"

#include "class.h"
#include "struct.h"
#include "rawtype.h"
#include "union.h"
#include "namespace.h"
#include "function.h"
#include "variable.h"
#include "enum.h"
#include "typedef.h"

#include "../macros.h"
#include "../assert.h"

#include "../malloc.h"

void rlc_parsed_scope_entry_destroy_base(
	struct RlcParsedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fNames)
	{
		rlc_free((void**)&this->fNames);
		this->fNameCount = 0;
	}
}


void rlc_parsed_scope_entry_destroy_virtual(
	struct RlcParsedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedScopeEntryType));

	typedef void const * (*destructor_t)(
		void * this);
	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_parsed_class_destroy,
		(destructor_t)&rlc_parsed_struct_destroy,
		(destructor_t)&rlc_parsed_rawtype_destroy,
		(destructor_t)&rlc_parsed_union_destroy,
		(destructor_t)&rlc_parsed_namespace_destroy,
		(destructor_t)&rlc_parsed_function_destroy,
		(destructor_t)&rlc_parsed_variable_destroy,
		(destructor_t)&rlc_parsed_enum_destroy,
		(destructor_t)&rlc_parsed_typedef_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedScopeEntryType), "ill-sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedClass),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedStruct),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedRawtype),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedUnion),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedNamespace),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedFunction),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedVariable),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedEnum),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedTypedef),
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedScopeEntryType), "ill-sized offset table.");

	k_vtable[RLC_DERIVING_TYPE(this)]((uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)]);
}

void rlc_parsed_scope_entry_create(
	struct RlcParsedScopeEntry * this,
	enum RlcParsedScopeEntryType derivingType)
{
	RLC_DASSERT(this != NULL);

	this->fDeclarationIndex = 0;
	this->fNames = NULL;
	this->fNameCount = 0;

	RLC_DERIVING_TYPE(this) = derivingType;
}

void rlc_parsed_scope_entry_add_name(
	struct RlcParsedScopeEntry * this,
	size_t name)
{
	RLC_DASSERT(this != NULL);

	rlc_realloc(
		(void**)&this->fNames,
		sizeof(size_t) * ++this->fNameCount);
	this->fNames[this->fNameCount-1] = name;
}

void rlc_parsed_scope_entry_list_create(
	struct RlcParsedScopeEntryList * this)
{
	RLC_DASSERT(this != NULL);

	this->fEntries = NULL;
	this->fEntryCount = 0;
}

void rlc_parsed_scope_entry_list_add(
	struct RlcParsedScopeEntryList * this,
	struct RlcParsedScopeEntry * entry)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(entry != NULL);

	rlc_realloc(
		(void**)&this->fEntries,
		sizeof(struct RlcParsedScopeEntry *) * ++this->fEntryCount);

	this->fEntries[this->fEntryCount -1] = 0;
}

void rlc_parsed_scope_entry_list_destroy(
	struct RlcParsedScopeEntryList * this)
{
	RLC_DASSERT(this != NULL);

	for(int i = 0; i < this->fEntryCount; i++)
	{
		rlc_parsed_scope_entry_destroy_virtual(
			this->fEntries[i]);
		rlc_free((void**)&this->fEntries[i]);
	}

	rlc_free((void**)&this->fEntries);
}