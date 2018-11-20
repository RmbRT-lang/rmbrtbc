#include "struct.h"

#include "../assert.h"

void rlc_parsed_struct_create(
	struct RlcParsedStruct * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedStruct,
		start_index);

	rlc_parsed_member_list_create(
		&this->fMembers);
}

void rlc_parsed_struct_destroy(
	struct RlcParsedStruct * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_member_list_destroy(
		&this->fMembers);
}

int rlc_parsed_struct_parse(
	struct RlcParsedStruct * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;
	size_t const start_index = parser->fIndex;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokStruct))
	{
		return 0;
	}

	rlc_parsed_struct_create(
		out,
		start_index);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	} else
	{
		rlc_parsed_scope_entry_add_name(
			RLC_BASE_CAST(out, RlcParsedScopeEntry),
			rlc_parser_data_consumed_index(parser));
	}

	if(!rlc_template_decl_parse(
		&out->fTemplates,
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

	for(enum RlcVisibility visibility = kRlcVisibilityPublic;;)
	{
		struct RlcParsedMember * member = rlc_parsed_member_parse(
			&visibility,
			parser,
			RLC_FLAG(kRlcParsedMemberRawtype)
			| RLC_FLAG(kRlcParsedMemberStruct)
			| RLC_FLAG(kRlcParsedMemberFunction)
			| RLC_FLAG(kRlcParsedMemberVariable));
		if(!member)
			break;
		else
			rlc_parsed_member_list_add(
				&out->fMembers,
				member);
	}

	if(parser->fErrorCount)
	{
		error_code = kRlcParseErrorExpectedStructMember;
		goto failure;
	}

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
	rlc_parsed_struct_destroy(out);
	rlc_parser_data_add_error(
		parser,
		error_code);
	return 0;
}

void rlc_parsed_member_struct_create(
	struct RlcParsedMemberStruct * this,
	enum RlcVisibility visiblity,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberStruct,
		visiblity,
		start_index);

	rlc_parsed_struct_create(
		RLC_BASE_CAST(this, RlcParsedStruct),
		0);
}

void rlc_parsed_member_struct_destroy(
	struct RlcParsedMemberStruct * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
	rlc_parsed_struct_destroy(
		RLC_BASE_CAST(this, RlcParsedStruct));
}

int rlc_parsed_member_struct_parse(
	struct RlcParsedMemberStruct * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(default_visibility != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start_index = parser->fIndex;
	enum RlcParseError error_code;

	enum RlcVisibility visibility = rlc_visibility_parse(
		default_visibility,
		parser);

	rlc_parsed_member_struct_create(
		out,
		start_index,
		visibility);

	if(!rlc_parsed_struct_parse(
		RLC_BASE_CAST(out, RlcParsedStruct),
		parser))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedStruct;
			goto failure;
		} else goto nonfatal_failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_member_struct_destroy(out);
nonfatal_failure:
	parser->fIndex = start_index;
	return 0;
}