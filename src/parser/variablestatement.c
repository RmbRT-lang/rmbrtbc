#include "variablestatement.h"


#include "../assert.h"

void rlc_parsed_variable_statement_create(
	struct RlcParsedVariableStatement * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedVariableStatement);

	rlc_parsed_variable_create(
		&this->fVariable,
		start_index);
}

void rlc_parsed_variable_statement_destroy(
	struct RlcParsedVariableStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_variable_destroy(&this->fVariable);
	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_variable_statement_parse(
	struct RlcParsedVariableStatement * out,
	struct RlcParserData * parser)
{

	rlc_parsed_variable_statement_create(
		out,
		rlc_parser_data_matched_index(parser));

	enum RlcParseError error_code;
	if(!rlc_parsed_variable_parse(
		&out->fVariable,
		parser,
		1,
		1,
		0,
		0))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedVariable;
			goto failure;
		} else return 0;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		rlc_parsed_variable_destroy(&out->fVariable);
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	// don't destroy the variable, it is already destroyed.
	return 0;
}