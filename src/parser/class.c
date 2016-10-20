#include "class.h"

#include "../assert.h"

void rlc_parsed_class_create(
	struct RlcParsedClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedClass);

	rlc_template_decl_create(&this->fTemplateDecl);

	rlc_parsed_member_list_create(&this->fMembers);
}

void rlc_parsed_class_destroy(
	struct RlcParsedClass * this)
{
	rlc_parsed_scope_entry_destroy_base(RLC_BASE_CAST(this, RlcParsedScopeEntry));
	
	rlc_template_decl_destroy(&this->fTemplateDecl);

	rlc_parsed_member_list_destroy(&this->fMembers);
}

void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	enum RlcVisibility visibility)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_class_create(
		RLC_BASE_CAST(this, RlcParsedClass));
	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberClass,
		visibility);
}

void rlc_parsed_member_class_destroy(
	struct RlcParsedMemberClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_class_destroy(
		RLC_BASE_CAST(this, RlcParsedClass));
	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}