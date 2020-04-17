#include "destructor.h"
#include "scopeitemgroup.h"

#include "../parser/destructor.h"

#include "../assert.h"

#define kRlcScopedDestructor kRlcParsedDestructor

void rlc_scoped_destructor_create(
	struct RlcScopedDestructor * this,
	struct RlcSrcFile const * file,
	struct RlcParsedDestructor const * parsed,
	struct RlcScopedScopeItemGroup * group)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(group != NULL);

	rlc_scoped_member_create(
		RLC_BASE_CAST(this, RlcScopedMember),
		file,
		RLC_BASE_CAST(parsed, RlcParsedMember),
		group->parent,
		kRlcScopedDestructor,
		NULL);

	this->isInline = parsed->fIsInline;
	if((this->isDefinition = parsed->fIsDefinition))
	{
		rlc_scoped_block_statement_create(
			&this->body,
			file,
			&parsed->fBody,
			group->parent);
	}
}

void rlc_scoped_destructor_destroy(
	struct RlcScopedDestructor * this)
{
	RLC_DASSERT(this != NULL);

	if(this->isDefinition)
	{
		rlc_scoped_block_statement_destroy(&this->body);
		this->isDefinition = 0;
	}

	rlc_scoped_member_destroy_base(
		RLC_BASE_CAST(this, RlcScopedMember));
}