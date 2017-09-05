#include "class.h"

#include "../assert.h"

void rlc_parsed_class_create(
	struct RlcParsedClass * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedClass,
		start_index);

	rlc_template_decl_create(&this->fTemplateDecl);

	rlc_parsed_member_list_create(&this->fMembers);
}

void rlc_parsed_class_destroy(
	struct RlcParsedClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_template_decl_destroy(&this->fTemplateDecl);

	rlc_parsed_member_list_destroy(&this->fMembers);
}

int rlc_parsed_class_parse(
	struct RlcParsedClass * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;
	size_t const start_index = parser->fIndex;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokClass))
	{
		return 0;
	}

	rlc_parsed_class_create(
		out,
		start_index);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	}

	rlc_parsed_scope_entry_add_name(
		RLC_BASE_CAST(out,RlcParsedScopeEntry),
		rlc_parser_data_consumed_index(parser));

	if(!rlc_template_decl_parse(
		&out->fTemplateDecl,
		parser))
	{
		error_code = kRlcParseErrorExpectedTemplateDeclaration;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokColonEqual))
	{
		error_code = kRlcParseErrorExpectedColonEqual;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
	{
		error_code = kRlcParseErrorExpectedBraceOpen;
		goto failure;
	}

	// default visibility is private.
	enum RlcVisibility visibility = kRlcVisibilityPrivate;
	struct RlcParsedMember * member = NULL;
	for(;;)
	{
		if(member = rlc_parsed_member_parse(
			&visibility,
			parser,
			RLC_ALL_FLAGS(RlcParsedMemberType))) // any kind of member is allowed inside classes.
		{
			// add the member to the members list.
			rlc_parsed_member_list_add(
				&out->fMembers,
				member);
		} else if(parser->fErrorCount) // was there a parsing error?
		{
			error_code = kRlcParseErrorExpectedClassMember;
			goto failure;
		} else break; // or was there just no member at all?
	}

	// finishing sequence of the class definition.
	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceClose))
	{
		error_code = kRlcParseErrorExpectedBraceClose;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedScopeEntry)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parsed_class_destroy(out);
	rlc_parser_data_add_error(
		parser,
		error_code);
	parser->fIndex = start_index;
	return 0;
}

void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	size_t start_index,
	enum RlcVisibility visibility)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_class_create(
		RLC_BASE_CAST(this, RlcParsedClass),
		0);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberClass,
		visibility,
		start_index);
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
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(default_visibility != NULL);

	size_t const start_index = parser->fIndex;
	enum RlcParseError error_code;

	enum RlcVisibility visibility = rlc_visibility_parse(
		default_visibility,
		parser);

	rlc_parsed_member_class_create(
		out,
		start_index,
		visibility);

	if(!rlc_parsed_class_parse(
		RLC_BASE_CAST(out, RlcParsedClass),
		parser))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedMemberClass;
			goto fatal_failure;
		} else goto nonfatal_failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
fatal_failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_member_class_destroy(out);
nonfatal_failure:
	parser->fIndex = start_index;
	return 0;
}