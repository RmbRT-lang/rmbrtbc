#include "rawtype.h"
#include "expression.h"
#include "member.h"
#include "scope.h"
#include "scopeitemgroup.h"
#include "../parser/rawtype.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedRawtype kRlcParsedRawtype
#define kRlcScopedMemberRawtype kRlcParsedMemberRawtype

static void rlc_scoped_rawtype_create(
	struct RlcScopedRawtype * this,
	struct RlcSrcFile const * file,
	struct RlcParsedRawtype const * parsed,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->size = rlc_scoped_expression_new(parsed->fSize, file);

	for(RlcSrcIndex i = 0; i < parsed->fMembers.fEntryCount; i++)
	{
		rlc_scoped_scope_add_member(
			scope,
			file,
			parsed->fMembers.fEntries[i]);
	}
}

static void rlc_scoped_rawtype_destroy(
	struct RlcScopedRawtype * this)
{
	rlc_scoped_expression_delete_virtual(this->size);
}

void rlc_scoped_global_rawtype_create(
	struct RlcScopedGlobalRawtype * this,
	struct RlcSrcFile const * file,
	struct RlcParsedRawtype const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedRawtype,
		&parsed->fTemplates);

	rlc_scoped_rawtype_create(
		RLC_BASE_CAST(this, RlcScopedRawtype),
		file,
		parsed,
		RLC_BASE_CAST2(this, RlcScopedScopeEntry, RlcScopedScopeItem)->children);
}

void rlc_scoped_global_rawtype_destroy(
	struct RlcScopedGlobalRawtype * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));

	rlc_scoped_rawtype_destroy(
		RLC_BASE_CAST(this, RlcScopedRawtype));
}

void rlc_scoped_member_rawtype_create(
	struct RlcScopedMemberRawtype * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberRawtype const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_member_create(
		RLC_BASE_CAST(this, RlcScopedMember),
		file,
		RLC_BASE_CAST(parsed, RlcParsedMember),
		parent->parent,
		kRlcScopedMemberRawtype,
		&RLC_BASE_CAST(parsed, RlcParsedRawtype)->fTemplates);

	rlc_scoped_rawtype_create(
		RLC_BASE_CAST(this, RlcScopedRawtype),
		file,
		RLC_BASE_CAST(parsed, RlcParsedRawtype),
		RLC_BASE_CAST2(this, RlcScopedMember, RlcScopedScopeItem)->children);
}

void rlc_scoped_member_rawtype_destroy(
	struct RlcScopedMemberRawtype * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));

	rlc_scoped_rawtype_destroy(
		RLC_BASE_CAST(this, RlcScopedRawtype));
}