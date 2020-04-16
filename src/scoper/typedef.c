#include "typedef.h"
#include "scopeitemgroup.h"
#include "../parser/typedef.h"
#include "../assert.h"

#define kRlcScopedTypedef kRlcParsedTypedef
#define kRlcScopedMemberTypedef kRlcParsedMemberTypedef

static void rlc_scoped_typedef_create(
	struct RlcScopedTypedef * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTypedef const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_type_name_create(&this->type, file, &parsed->fType);
}

static void rlc_scoped_typedef_destroy(
	struct RlcScopedTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_type_name_destroy(&this->type);
}

void rlc_scoped_global_typedef_create(
	struct RlcScopedGlobalTypedef * this,
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

	rlc_scoped_typedef_create(
		RLC_BASE_CAST(this, RlcScopedTypedef),
		file,
		parsed);
}

void rlc_scoped_global_typedef_destroy(
	struct RlcScopedGlobalTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_scoped_typedef_destroy(RLC_BASE_CAST(this, RlcScopedTypedef));
}

void rlc_scoped_member_typedef_create(
	struct RlcScopedMemberTypedef * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberTypedef const * parsed,
	struct RlcScopedScopeItemGroup * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(scope != NULL);

	rlc_scoped_member_create(
		RLC_BASE_CAST(this, RlcScopedMember),
		file,
		RLC_BASE_CAST(parsed, RlcParsedMember),
		scope->parent,
		kRlcScopedMemberTypedef,
		&RLC_BASE_CAST(parsed, RlcParsedTypedef)->fTemplates);

	rlc_scoped_typedef_create(
		RLC_BASE_CAST(this, RlcScopedTypedef),
		file,
		RLC_BASE_CAST(parsed, RlcParsedTypedef));
}

void rlc_scoped_member_typedef_destroy(
	struct RlcScopedMemberTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));

	rlc_scoped_typedef_destroy(
		RLC_BASE_CAST(this, RlcScopedTypedef));
}