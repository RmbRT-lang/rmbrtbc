#include "typedef.h"
#include "parser.h"
#include "../assert.h"


int rlc_parsed_typedef_parse(
	struct RlcParsedTypedef * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);


	enum RlcParseError error_code;
	size_t const start_index = parser->fIndex;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokType))
	{
		return 0;
	}

	rlc_parsed_typedef_create(
		out,
		start_index);
	RLC_BASE_CAST(out, RlcParsedScopeEntry)->fLocation.fBegin = start_index;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIdentifier))
	{
		error_code = kRlcParseErrorExpectedIdentifier;
		goto failure;
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

	if(!rlc_parsed_type_name_parse(
		&out->fType,
		parser))
	{
		error_code = kRlcParseErrorExpectedTypeName;
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
	rlc_parsed_typedef_destroy(out);
	parser->fIndex = start_index;
	return 0;
}

void rlc_parsed_typedef_create(
	struct RlcParsedTypedef * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedTypedef,
		start_index);

	rlc_template_decl_create(
		&this->fTemplates);

	rlc_parsed_type_name_create(
		&this->fType);
}

void rlc_parsed_typedef_destroy(
	struct RlcParsedTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_type_name_destroy(
		&this->fType);

	rlc_template_decl_destroy(
		&this->fTemplates);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));
}


void rlc_parsed_member_typedef_create(
	struct RlcParsedMemberTypedef * this,
	enum RlcVisibility visibility,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberTypedef,
		visibility,
		start_index);
	rlc_parsed_typedef_create(
		RLC_BASE_CAST(this, RlcParsedTypedef),
		0);
}

void rlc_parsed_member_typedef_destroy(
	struct RlcParsedMemberTypedef * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_typedef_destroy(
		RLC_BASE_CAST(this, RlcParsedTypedef));

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

int rlc_parsed_member_typedef_parse(
	struct RlcParsedMemberTypedef * out,
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

	rlc_parsed_member_typedef_create(
		out,
		visibility,
		start_index);

	if(!rlc_parsed_typedef_parse(
		RLC_BASE_CAST(out, RlcParsedTypedef),
		parser))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedTypedef;
			goto failure;
		}
		goto nonfatal_failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_member_typedef_destroy(out);
nonfatal_failure:
	parser->fIndex = start_index;
	return 0;
}