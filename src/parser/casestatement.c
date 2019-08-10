#include "casestatement.h"

#include "../assert.h"

void rlc_parsed_case_statement_create(
	struct RlcParsedCaseStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedCaseStatement);

	this->fIsDefault = 1;
	rlc_control_label_create(&this->fControlLabel);
	rlc_parsed_expression_list_create(&this->fValues);
	rlc_parsed_statement_list_create(&this->fStatements);

}

void rlc_parsed_case_statement_destroy(
	struct RlcParsedCaseStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_list_destroy(&this->fValues);
	this->fIsDefault = 1;


	rlc_control_label_destroy(&this->fControlLabel);
	rlc_parsed_statement_list_destroy(&this->fStatements);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_case_statement_parse(
	struct RlcParsedCaseStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;
	enum RlcParseError error_code;

	rlc_parsed_case_statement_create(out);

	if(rlc_parser_data_consume(
		parser,
		kRlcTokDefault))
	{
		out->fIsDefault = 1;
		if(!rlc_control_label_parse(
			&out->fControlLabel,
			parser)
		&& parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedControlLabel;
			goto failure;
		}
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokCase))
	{
		if(!rlc_control_label_parse(
			&out->fControlLabel,
			parser)
		&& parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedControlLabel;
			goto failure;
		}
		out->fIsDefault = 0;

		do {
			struct RlcParsedExpression * value = rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType));

			if(!value) {
				error_code = kRlcParseErrorExpectedExpression;
				goto failure;
			}

			rlc_parsed_expression_list_append(
				&out->fValues,
				value);
		} while(rlc_parser_data_consume(
			parser,
			kRlcTokComma));
	} else return 0;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokColon))
	{
		error_code = kRlcParseErrorExpectedColon;
		goto failure;
	}

	struct RlcParsedStatement * stmt;
	while(stmt = rlc_parsed_statement_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedStatementType)
		&~RLC_FLAG(kRlcParsedCaseStatement)))
	{
		rlc_parsed_statement_list_add(
			&out->fStatements,
			stmt);
	}

	if(parser->fErrors
	|| rlc_parser_data_match(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedStatement;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_case_statement_destroy(out);
	parser->fIndex = start;
	return 0;
}