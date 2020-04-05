#include "typedef.h"
#include "../parser/typedef.h"
#include "../assert.h"

#define kRlcScopedTypedef kRlcParsedTypedef

void rlc_scoped_typedef_create(
	struct RlcScopedTypedef * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTypedef const * parsed,
	struct RlcScopedScopeItemGroup * group)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(group != NULL);

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		group,
		kRlcScopedTypedef,
		&parsed->fTemplates);

	rlc_scoped_type_name_create(&this->type, file, &parsed->fType);
}

void rlc_scoped_typedef_destroy(
	struct RlcScopedTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_scoped_type_name_destroy(&this->type);
}