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
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokIf))
		return 0;

	rlc_parsed_if_statement_create(out);

	rlc_parsed_control_label_parse(
		&out->fIfLabel,
		parser);

	out->fIsNegated = rlc_parser_consume(parser, NULL, kRlcTokExclamationMark);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if((out->fCondition.fIsVariable = rlc_parsed_variable_parse(
		&out->fCondition.fVariable,
		parser,
		NULL,
		1,
		1,
		1,
		0,
		0)))
	{
		;
	} else if((out->fCondition.fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		;
	} else
	{
		rlc_parser_fail(parser, "expected condition");
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);

	if(!(out->fIf = rlc_parsed_statement_parse(
		parser,
		kBodyStatementFlags)))
	{
		rlc_parser_fail(parser, "expected if body statement");
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokElse))
	{
		if(!(out->fElse = rlc_parsed_statement_parse(
			parser,
			kBodyStatementFlags)))
		{
			rlc_parser_fail(parser, "expected else body statement");
		}
	}

	return 1;
}

void rlc_parsed_if_statement_print(
	struct RlcParsedIfStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fCondition.fIsVariable)
	{
		fputs("{", out);
		rlc_parsed_variable_print_argument(
			&this->fCondition.fVariable,
			file,
			out,
			1);

		fputs(";", out);

		fputs(this->fIsNegated? "if(!": "if(", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(&this->fCondition.fVariable, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs(")", out);
	} else
	{
		fputs(this->fIsNegated ? "if(!(" : "if(", out);
		rlc_parsed_expression_print(this->fCondition.fExpression, file, out);
		fputs(this->fIsNegated ? "))" : ")", out);
	}

	rlc_parsed_statement_print(this->fIf, file, out);
	if(this->fElse)
	{
		fputs("else\n\t", out);
		rlc_parsed_statement_print(this->fElse, file, out);
	}

	if(this->fCondition.fIsVariable)
		fputs("}", out);
	rlc_parsed_control_label_print(&this->fIfLabel, file, out, "_break");
}