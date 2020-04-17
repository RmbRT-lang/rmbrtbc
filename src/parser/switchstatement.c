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

	this->fIsVariableSwitchValue = 0;
	this->fSwitchValue.fExpression = NULL;
	this->fCases = NULL;
	this->fCaseCount = 0;
}

void rlc_parsed_switch_statement_destroy(
	struct RlcParsedSwitchStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fIsVariableSwitchValue)
	{
		rlc_parsed_variable_destroy(&this->fSwitchValue.fVariable);
	} else if(this->fSwitchValue.fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fSwitchValue.fExpression);
		rlc_free((void**)&this->fSwitchValue.fExpression);
	}

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
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokSwitch))
		return 0;

	rlc_parsed_switch_statement_create(out);
	rlc_parsed_control_label_parse(&out->fLabel, parser);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(!(out->fIsVariableSwitchValue = rlc_parsed_variable_parse(
		&out->fSwitchValue.fVariable,
		parser,
		NULL,
		1,
		1,
		1,
		0,
		0))
	&& !(out->fSwitchValue.fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		rlc_parser_fail(parser, "expected expression");
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceOpen);

	do {
		struct RlcParsedCaseStatement case_stmt;
		if(!rlc_parsed_case_statement_parse(
			&case_stmt,
			parser))
		{
			rlc_parser_fail(parser, "expected 'case' statement");
		}
		rlc_parsed_switch_statement_add_case(
			out,
			&case_stmt);
	} while(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokBraceClose));

	return 1;
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