#include "ifstatement.h"

#include "../assert.h"
#include "../malloc.h"

static int const kBodyStatementFlags =
	RLC_ALL_FLAGS(RlcParsedStatementType)
^	RLC_FLAG(kRlcParsedCaseStatement)
^	RLC_FLAG(kRlcParsedVariableStatement);

void rlc_parsed_if_statement_create(
	struct RlcParsedIfStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedIfStatement);

	this->fCondition.fIsVariable = 0;
	this->fCondition.fExpression = NULL;

	rlc_control_label_create(&this->fIfLabel);
	rlc_control_label_create(&this->fElseLabel);

	this->fIf = NULL;
	this->fElse = NULL;
}

void rlc_parsed_if_statement_destroy(
	struct RlcParsedIfStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fCondition.fIsVariable)
	{
		rlc_parsed_variable_destroy(&this->fCondition.fVariable);
	} else if(this->fCondition.fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fCondition.fExpression);
		rlc_free((void**)&this->fCondition.fExpression);
	}

	if(this->fIf)
	{
		rlc_parsed_statement_destroy_virtual(this->fIf);
		rlc_free((void**)&this->fIf);
	}

	if(this->fElse)
	{
		rlc_parsed_statement_destroy_virtual(this->fElse);
		rlc_free((void**)&this->fElse);
	}
}

int rlc_parsed_if_statement_parse(
	struct RlcParsedIfStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokIf))
		return 0;

	enum RlcParseError error_code;

	rlc_parsed_if_statement_create(out);

	if(!rlc_control_label_parse(
		&out->fIfLabel,
		parser))
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

	if(out->fCondition.fIsVariable = rlc_parsed_variable_parse(
		&out->fCondition.fVariable,
		parser,
		1,
		1,
		1,
		0))
	{
	} else if(!(out->fCondition.fExpression = rlc_parsed_expression_parse(
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

	if(!(out->fIf = rlc_parsed_statement_parse(
		parser,
		kBodyStatementFlags)))
	{
		error_code = kRlcParseErrorExpectedBodyStatement;
		goto failure;
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokElse))
	{
		if(!rlc_control_label_parse(
			&out->fElseLabel,
			parser))
		{
			error_code = kRlcParseErrorExpectedControlLabel;
			goto failure;
		}
		if(!out->fIfLabel.fExists
		&& out->fElseLabel.fExists)
		{
			parser->fLatestIndex -= 2;
			parser->fIndex = parser->fLatestIndex;
			error_code = kRlcParseErrorUnexpectedControlLabel;
			goto failure;
		}

		if(out->fElseLabel.fExists
		&& !rlc_parser_data_equal_tokens(
			parser,
			out->fIfLabel.fLabel,
			out->fElseLabel.fLabel))
		{
			parser->fLatestIndex -= 2;
			parser->fIndex = parser->fLatestIndex;
			error_code = kRlcParseErrorMismatchedControlLabelName;
			goto failure;
		}

		if(!(out->fElse = rlc_parsed_statement_parse(
			parser,
			kBodyStatementFlags))
		&& parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedBodyStatement;
			goto failure;
		}
	}

success:
	return 1;
failure:
	rlc_parser_data_add_error(parser, error_code);
	rlc_parsed_if_statement_destroy(out);
	return 0;
}