#include "class.h"

#include "../assert.h"

void rlc_parsed_class_create(
	struct RlcParsedClass * this,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedClass,
		name);

	rlc_parsed_template_decl_create(&this->fTemplateDecl);

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
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken name;
	if((!rlc_parser_is_ahead(parser, kRlcTokBraceOpen)
		&& !rlc_parser_is_ahead(parser, kRlcTokMinusGreater))
	|| !rlc_parser_consume(parser, &name, kRlcTokIdentifier))
	{
		return 0;
	}

	rlc_parsed_class_create(out, &name.content);

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "class", &tracer);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceOpen);

	// default visibility is private.
	enum RlcVisibility visibility = kRlcVisibilityPrivate;
	struct RlcParsedDestructor destructor;
	while(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceClose))
	{
		if(rlc_parsed_destructor_parse(
			&destructor,
			&visibility,
			parser))
		{
			if(out->fHasDestructor)
				rlc_parser_fail(parser, "duplicate destructor");

			out->fHasDestructor = 1;
			out->fDestructor = destructor;
		} else
		{
			// add the member to the members list.
			rlc_parsed_member_list_add(
				&out->fMembers,
				rlc_parsed_member_parse(
					&visibility,
					parser,
					RLC_ALL_FLAGS(RlcParsedMemberType)
					&~RLC_FLAG(kRlcParsedDestructor)));
		}
	}

	return 1;
}

void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	enum RlcVisibility visibility)
{
	RLC_DASSERT(this != NULL);

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

int rlc_parsed_member_class_parse(
	struct RlcParsedMemberClass * out,
	enum RlcVisibility * default_visibility,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(default_visibility != NULL);

	enum RlcVisibility visibility = rlc_visibility_parse(
		default_visibility,
		parser);

	rlc_parsed_member_class_create(
		out,
		visibility);

	if(!rlc_parsed_class_parse(
		RLC_BASE_CAST(out, RlcParsedClass),
		parser))
	{
		return 0;
	}

	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
}