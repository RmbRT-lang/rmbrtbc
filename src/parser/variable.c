#include "variable.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_variable_destroy(
	struct RlcParsedVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_type_name_destroy(&this->fType);

	if(this->fInitExp)
	{
		rlc_parsed_expression_destroy_virtual(this->fInitExp);
		rlc_free((void**)&this->fInitExp);
	}
}

int rlc_parsed_variable_parse(
	struct RlcParsedVariable * out,
	struct RlcParserData * parser,
	int allow_initialiser,
	int allow_comma)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_member_variable_create(out);

	if(!rlc_parser_data_match(
		parser,
		kRlcTokIdentifier)
	|| !rlc_parser_data_match_ahead(
		parser,
		kRlcTokColon))
		return 0;

	rlc_parsed_variable_create(out);

	rlc_parsed_scope_entry_add_name(
		RLC_BASE_CAST(out, RlcParsedScopeEntry),
		parser->fIndex);

	rlc_parser_data_next(parser);
	rlc_parser_data_next(parser);

	rlc_parsed_type_name_parse(
		&out->fType,
		parser);

	if(allow_initialiser
	&& rlc_parser_data_consume(
		parser,
		kRlcTokColonEqual)
	&& !rlc_parsed_expression_parse(
		&out->fInitExp,
		parser,
		allow_comma))
	{
		return rlc_parser_data_add_error(
			parser,
			allow_comma
				? kRlcParseErrorExpectedExpressionNoComma
				: kRlcParseErrorExpectedExpression), 0;
	}

	return 1;
}


int rlc_parsed_member_variable_parse(
	struct RlcParsedMemberVariable * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	out->fIsolated = rlc_parser_data_consume(
		parser,
		kRlcTokIsolated));

	if(!rlc_parsed_variable_parse(
		RLC_BASE_CAST(out, RlcParsedVariable),
		parser))
	{
		// something was parsed already?
		if(parser->fErrorCount || out->fIsolated)
		{
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedVariable)
		}

		return 0;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedSemicolon), 0;

	return 1;
}