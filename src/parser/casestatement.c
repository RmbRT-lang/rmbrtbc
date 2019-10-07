#include "casestatement.h"

#include "../assert.h"
#include "../malloc.h"

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
	this->fBody = NULL;

}

void rlc_parsed_case_statement_destroy(
	struct RlcParsedCaseStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_list_destroy(&this->fValues);
	this->fIsDefault = 1;


	rlc_control_label_destroy(&this->fControlLabel);
	if(this->fBody)
	{
		rlc_parsed_statement_destroy_virtual(this->fBody);
		rlc_free((void**)&this->fBody);
	}

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_case_statement_parse(
	struct RlcParsedCaseStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_case_statement_create(out);

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDefault))
	{
		out->fIsDefault = 1;
		rlc_control_label_parse(
			&out->fControlLabel,
			parser);
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokCase))
	{
		rlc_control_label_parse(
			&out->fControlLabel,
			parser);
		out->fIsDefault = 0;

		do
		{
			rlc_parsed_expression_list_append(
				&out->fValues,
				rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType)));
		} while(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokComma));
	} else return 0;

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokColon);

	out->fBody = rlc_parsed_statement_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedStatementType)
		&~RLC_FLAG(kRlcParsedCaseStatement)
		&~RLC_FLAG(kRlcParsedVariableStatement));

	return 1;
}