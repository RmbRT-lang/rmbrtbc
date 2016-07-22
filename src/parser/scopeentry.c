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

#include <assert.h>

void rlc_parsed_scope_entry_destroy(
	struct RlcParsedScopeEntry * this)
{
	assert(this != NULL);
	assert(RLC_IN_ENUM(this->fRlcParsedScopeEntry, RlcParsedScopeEntryType));

	typedef void (*destructor_t)(struct RlcParsedScopeEntry *);
	static destructor_t const vtable[] = {
		(destructor_t)&rlc_parsed_class_destroy,
		(destructor_t)&rlc_parsed_struct_destroy,
		(destructor_t)&rlc_parsed_union_destroy,
		(destructor_t)&rlc_parsed_namespace_destroy,
		(destructor_t)&rlc_parsed_function_destroy,
		(destructor_t)&rlc_parsed_variable_destroy,
		(destructor_t)&rlc_parsed_enum_destroy,
		(destructor_t)&rlc_parsed_typedef_destroy
	};

	assert(RLC_COVERS_ENUM(vtable, RlcParsedScopeEntryType));

	vtable[this->fRlcParsedScopeEntry](this);
}