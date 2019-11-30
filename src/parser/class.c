#include "class.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_class_create(
	struct RlcParsedClass * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedClass,
		name);

	if(templates)
		this->fTemplateDecl = *templates;
	else
		rlc_parsed_template_decl_create(&this->fTemplateDecl);

	this->fInheritances = NULL;
	this->fInheritanceCount = 0;

	rlc_parsed_member_list_create(&this->fMembers);

	this->fHasDestructor = 0;
}

void rlc_parsed_class_destroy(
	struct RlcParsedClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_template_decl_destroy(&this->fTemplateDecl);

	rlc_parsed_member_list_destroy(&this->fMembers);

	if(this->fHasDestructor)
	{
		rlc_parsed_destructor_destroy(&this->fDestructor);
		this->fHasDestructor = 0;
	}
}

int rlc_parsed_class_parse(
	struct RlcParsedClass * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(templates != NULL);

	struct RlcToken name;
	if((!rlc_parser_is_ahead(parser, kRlcTokBraceOpen)
		&& !rlc_parser_is_ahead(parser, kRlcTokMinusGreater))
	|| !rlc_parser_consume(parser, &name, kRlcTokIdentifier))
	{
		return 0;
	}

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "class", &tracer);

	rlc_parsed_class_create(out, &name.content, templates);

	if(kRlcTokMinusGreater == rlc_parser_expect(
		parser,
		NULL,
		2,
		kRlcTokMinusGreater,
		kRlcTokBraceOpen))
	{
		do
		{
			rlc_realloc(
				(void**)&out->fInheritances,
				++out->fInheritanceCount * sizeof(struct RlcParsedInheritance));
			struct RlcParsedInheritance * in =
				&out->fInheritances[out->fInheritanceCount-1];

			int _ = rlc_visibility_parse(&in->fVisibility, parser, kRlcVisibilityPublic);
			(void) _;

			in->fVirtual = rlc_parser_consume(
				parser,
				NULL,
				kRlcTokVirtual);
			if(!rlc_parsed_symbol_parse(&in->fBase, parser))
				rlc_parser_fail(parser, "expected symbol");
		} while(kRlcTokComma == rlc_parser_expect(
			parser,
			NULL,
			2,
			kRlcTokComma,
			kRlcTokBraceOpen));
	}

	struct RlcParsedMemberCommon common;
	rlc_parsed_member_common_create(&common, kRlcVisibilityPublic);

	struct RlcParsedMember * member;
	while((member = rlc_parsed_member_parse(
		parser,
		&common,
		RLC_ALL_FLAGS(RlcParsedMemberType)
		& (out->fHasDestructor ? ~RLC_FLAG(kRlcParsedDestructor) : ~0))))
	{
		if(RLC_DERIVING_TYPE(member) == kRlcParsedDestructor)
		{
			out->fHasDestructor = 1;
			out->fDestructor = *RLC_DERIVE_CAST(
				member,
				RlcParsedMember,
				struct RlcParsedDestructor);
			rlc_free((void**)&member);
		} else
		{
			// add the member to the members list.
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

	rlc_parser_untrace(parser, &tracer);

	return 1;
}

void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);
	RLC_DASSERT(member->attribute == kRlcMemberAttributeNone);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberClass,
		member);
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

int rlc_parsed_member_class_parse(
	struct RlcParsedMemberClass * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(member != NULL);

	if(member->attribute != kRlcMemberAttributeNone
	|| !rlc_parsed_class_parse(
		RLC_BASE_CAST(out, RlcParsedClass),
		parser,
		&member->templates))
		return 0;

	rlc_parsed_member_class_create(
		out,
		member);

	return 1;
}