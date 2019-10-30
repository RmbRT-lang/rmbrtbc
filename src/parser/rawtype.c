#include "rawtype.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_rawtype_create(
	struct RlcParsedRawtype * this,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedRawtype,
		name);

	this->fSize = NULL;

	rlc_parsed_member_list_create(
		&this->fMembers);
	rlc_parsed_template_decl_create(
		&this->fTemplates);
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
	struct RlcParsedTemlateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokRawtype))
	{
		goto nonfatal_failure;
	}

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


	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBracketOpen))
	{
		error_code = kRlcParseErrorExpectedBracketOpen;
		goto failure;
	}

	if(!(out->fSize = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		error_code = kRlcParseErrorExpectedExpression;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBracketClose))
	{
		error_code = kRlcParseErrorExpectedBracketClose;
		goto failure;
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
		goto success;

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
			RLC_FLAG(kRlcParsedMemberFunction));
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
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_rawtype_destroy(out);
	return 0;
}

void rlc_parsed_member_rawtype_create(
	struct RlcParsedMemberRawtype * this,
	enum RlcVisibility visibility,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(visibility, RlcVisibility));

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberRawtype,
		visibility,
		start_index);

	rlc_parsed_rawtype_create(
		RLC_BASE_CAST(this, RlcParsedRawtype),
		0);
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
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(default_visibility != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start_index = parser->fIndex;
	enum RlcParseError error_code;
	enum RlcVisibility visibility = rlc_visibility_parse(
		default_visibility,
		parser);

	rlc_parsed_member_rawtype_create(
		out,
		member);

	if(!rlc_parsed_rawtype_parse(
		RLC_BASE_CAST(out, RlcParsedRawtype),
		parser))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedRawtype;
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
	rlc_parsed_member_rawtype_destroy(out);
nonfatal_failure:
	parser->fIndex = start_index;
	return 0;
}