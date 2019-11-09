#include "rawtype.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_rawtype_create(
	struct RlcParsedRawtype * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedRawtype,
		name);

	this->fSize = NULL;

	rlc_parsed_member_list_create(
		&this->fMembers);
	this->fTemplates = *templates;
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

	rlc_parsed_template_decl_destroy(
		&this->fTemplates);
}

int rlc_parsed_rawtype_parse(
	struct RlcParsedRawtype * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokParentheseOpen))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "rawtype", &tracer);

	if(!(out->fSize = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		rlc_parser_fail(parser, "expected expression");
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_rawtype_create(out, &name.content, templates);

	if(kRlcTokSemicolon == rlc_parser_expect(
		parser,
		NULL,
		2,
		kRlcTokSemicolon,
		kRlcTokBraceOpen))
	{
		rlc_parser_untrace(parser, &tracer);
		return 1;
	}

	struct RlcParsedMemberCommon common;
	rlc_parsed_member_common_create(&common, kRlcVisibilityPublic);
	while(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokParentheseClose))
	{
		struct RlcParsedMember * member = rlc_parsed_member_parse(
			parser,
			&common,
			RLC_FLAG(kRlcParsedMemberFunction));

		if(!member)
			rlc_parser_fail(parser, "expected member function");

		rlc_parsed_member_list_add(
			&out->fMembers,
			member);
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceClose);

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_member_rawtype_create(
	struct RlcParsedMemberRawtype * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberRawtype,
		member);
}

void rlc_parsed_member_rawtype_destroy(
	struct RlcParsedMemberRawtype * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_rawtype_destroy(
		RLC_BASE_CAST(this, RlcParsedRawtype));

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

int rlc_parsed_member_rawtype_parse(
	struct RlcParsedMemberRawtype * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(member != NULL);

	if(!rlc_parsed_rawtype_parse(
		RLC_BASE_CAST(out, RlcParsedRawtype),
		parser,
		&member->templates))
	{
		return 0;
	}

	rlc_parsed_member_rawtype_create(
		out,
		member);

	return 1;
}