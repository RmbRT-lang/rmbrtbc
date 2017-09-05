#include "union.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_union_create(
	struct RlcParsedUnion * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedUnion,
		start_index);

	rlc_parsed_member_list_create(
		&this->fMembers);
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
	struct RlcParsedUnion * this,
	struct RlcParserData * parser)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;
	size_t const start_index = parser->fIndex;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokUnion))
	{
		return 0;
	}

	rlc_parsed_union_create(
		this,
		start_index);


	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
	}
	else
	{
		rlc_parsed_scope_entry_add_name(
			RLC_BASE_CAST(this, RlcParsedScopeEntry),
			rlc_parser_data_consumed_index(parser));
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

	{
		struct RlcParsedMember * member = NULL;
		enum RlcVisibility visibility = kRlcVisibilityPublic;
		while((member = rlc_parsed_member_parse(
			&visibility,
			parser,
			RLC_FLAG(kRlcParsedMemberVariable))))
		{
			rlc_parsed_member_list_add(
				&this->fMembers,
				member);
		}
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
	RLC_BASE_CAST(this, RlcParsedScopeEntry)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parsed_union_destroy(
		this);
	rlc_parser_data_add_error(
		parser,
		error_code);
	return 0;
}

void rlc_parsed_member_union_create(
	struct RlcParsedMemberUnion * this,
	enum RlcVisibility visibility,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberUnion,
		visibility,
		start_index);

	rlc_parsed_union_create(
		RLC_BASE_CAST(this, RlcParsedUnion),
		0);
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

	rlc_parsed_member_union_create(
		out,
		visibility,
		start_index);

	if(!rlc_parsed_union_parse(
		RLC_BASE_CAST(out, RlcParsedUnion),
		parser))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedUnion;
			goto failure;
		}
		else goto nonfatal_failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_member_union_destroy(out);
nonfatal_failure:
	parser->fIndex = start_index;
	return 0;
}