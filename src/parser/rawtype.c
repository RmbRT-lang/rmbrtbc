#include "rawtype.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_rawtype_create(
	struct RlcParsedRawtype * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedRawtype);

	this->fSize = NULL;
}

void rlc_parsed_rawtype_destroy(
	struct RlcParsedRawtype * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fSize)
	{
		rlc_parsed_expression_destroy_virtual(this->fSize);
		rlc_free((void**)&this->fSize);
	}

	rlc_parsed_member_list_destroy(&this->fMembers);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));
}