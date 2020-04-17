#include "union.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_union_create(
	struct RlcParsedUnion * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedUnion,
		name);

	rlc_parsed_member_list_create(
		&this->fMembers);

	this->fTemplates = *templates;
}

void rlc_parsed_union_destroy(
	struct RlcParsedUnion * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_list_destroy(
		&this->fMembers);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));
}

int rlc_parsed_union_parse(
	struct RlcParsedUnion * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(templates != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokUnion))
	{
		return 0;
	}

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_union_create(
		out,
		&name.content,
		templates);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceOpen);

	{
		struct RlcParsedMemberCommon common;
		rlc_parsed_member_common_create(&common, kRlcVisibilityPublic);
		struct RlcParsedMember * member = NULL;
		while((member = rlc_parsed_member_parse(
			parser,
			&common,
			RLC_FLAG(kRlcParsedMemberVariable)
			| RLC_FLAG(kRlcParsedMemberFunction))))
		{
			rlc_parsed_member_list_add(
				&out->fMembers,
				member);
		}
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceClose);

	return 1;
}

void rlc_parsed_member_union_create(
	struct RlcParsedMemberUnion * this,
	struct RlcParsedMemberCommon const * common)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(common != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberUnion,
		common);
}

void rlc_parsed_member_union_destroy(
	struct RlcParsedMemberUnion * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));

	rlc_parsed_union_destroy(
		RLC_BASE_CAST(this, RlcParsedUnion));
}

int rlc_parsed_member_union_parse(
	struct RlcParsedMemberUnion * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * common)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(common != NULL);

	if(!rlc_parsed_union_parse(
		RLC_BASE_CAST(out, RlcParsedUnion),
		parser,
		&common->templates))
	{
		return 0;
	}

	rlc_parsed_member_union_create(out, common);

	return 1;
}