#include "switchstatement.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_switch_statement_create(
	struct RlcParsedSwitchStatement * this)
{
	RLC_DASSERT(this != NULL);
	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedSwitchStatement);

	this->fSwitchValue = NULL;
	rlc_control_label_create(&this->fLabel);
	this->fCases = NULL;
	this->fCaseCount = 0;
}

void rlc_parsed_switch_statement_destroy(
	struct RlcParsedSwitchStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fSwitchValue)
	{
		rlc_parsed_expression_destroy_virtual(this->fSwitchValue);
		rlc_free((void**)&this->fSwitchValue);
	}

	rlc_control_label_destroy(&this->fLabel);

	if(this->fCases)
	{
		for(size_t i = 0; i < this->fCaseCount; i++)
			rlc_parsed_case_statement_destroy(&this->fCases[i]);
		rlc_free((void**)&this->fCases);
	}
	this->fCaseCount = 0;

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_switch_statement_parse(
	struct RlcParsedSwitchStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSwitch))
		return 0;

	size_t const start = parser->fIndex;
	enum RlcParseError error_code;

	rlc_parsed_switch_statement_create(out);
	if(!rlc_control_label_parse(&out->fLabel, parser))
	{
		error_code = kRlcParseErrorExpectedControlLabel;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		error_code = kRlcParseErrorExpectedParentheseOpen;
		goto failure;
	}

	if(!(out->fSwitchValue = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType)
	&~	RLC_FLAG(kRlcParsedTypeNameExpression))))
	{
		error_code = kRlcParseErrorExpectedExpression;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseClose))
	{
		error_code = kRlcParseErrorExpectedParentheseClose;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceOpen))
	{
		error_code = kRlcParseErrorExpectedBraceOpen;
		goto failure;
	}

	do {
		struct RlcParsedCaseStatement case_stmt;
		if(!rlc_parsed_case_statement_parse(
			&case_stmt,
			parser))
		{
			error_code = kRlcParseErrorExpectedCaseStatement;
			goto failure;
		}
		rlc_parsed_switch_statement_add_case(
			out,
			&case_stmt);
	} while(!rlc_parser_data_consume(
		parser,
		kRlcTokBraceClose));

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	parser->fIndex = start;
	rlc_parsed_switch_statement_destroy(out);
	return 0;
}

void rlc_parsed_switch_statement_add_case(
	struct RlcParsedSwitchStatement * this,
	struct RlcParsedCaseStatement * statement)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(statement != NULL);

	rlc_realloc(
		(void**)&this->fCases,
		sizeof(struct RlcParsedCaseStatement) * ++this->fCaseCount);
	this->fCases[this->fCaseCount-1] = *statement;
}