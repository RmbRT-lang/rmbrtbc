#include "destructor.h"

#include "../assert.h"

void rlc_parsed_destructor_create(
	struct RlcParsedDestructor * this)
{
	RLC_DASSERT(this != NULL);

	RLC_BASE_CAST(this, RlcParsedMember)->fVisibility = kRlcVisibilityPublic;
	rlc_parsed_block_statement_create(&this->fBody);
}


void rlc_parsed_destructor_destroy(
	struct RlcParsedDestructor * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_block_statement_destroy(&this->fBody);
}

int rlc_parsed_destructor_parse(
	struct RlcParsedDestructor * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;
	rlc_parsed_destructor_create(out);

	RLC_BASE_CAST(out, RlcParsedMember)->fVisibility = rlc_visibility_parse(
		default_visibility,
		parser);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokDestructor))
	{
		parser->fIndex = start;
		return 0;
	}

	enum RlcParseError error_code;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokColonEqual))
	{
		error_code = kRlcParseErrorExpectedColonEqual;
		goto failure;
	}

	if(!rlc_parsed_block_statement_parse(
		&out->fBody,
		parser))
	{
		error_code = kRlcParseErrorExpectedBlockStatement;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_destructor_destroy(out);
	return 0;
}