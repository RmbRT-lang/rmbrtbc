#include "switchstatement.h"
#include "blockstatement.h"

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
	this->fCommonIntro = NULL;
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

	if(this->fCommonIntro)
	{
		rlc_parsed_block_statement_destroy(this->fCommonIntro);
		rlc_free((void**)&this->fCommonIntro);
	}

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
	out->fIsStrict = !rlc_parser_consume(parser, NULL, kRlcTokQuestionMark);

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "switch statement", &tracer);

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

	int hasDefault = 0;

	if(rlc_parser_consume(parser, NULL, kRlcTokCommon))
	{
		struct RlcParserTracer tracer;
		rlc_parser_trace(parser, "COMMON case", &tracer);

		rlc_parser_expect(parser, NULL, 1, kRlcTokColon);

		struct RlcParsedBlockStatement common;
		if(!rlc_parsed_block_statement_parse(&common, parser))
			rlc_parser_fail(parser, "expected block statement");

		rlc_malloc((void**)&out->fCommonIntro, sizeof(common));
		*out->fCommonIntro = common;

		rlc_parser_untrace(parser, &tracer);
	}

	struct RlcParsedCaseStatement case_stmt;
	do {
		if(!rlc_parsed_case_statement_parse(
			&case_stmt,
			parser))
		{
			rlc_parser_fail(parser, "expected 'case' statement");
		}

		if(case_stmt.fIsDefault)
		{
			if(hasDefault)
				rlc_parser_fail(parser, "duplicate default case");
			if(out->fCommonIntro)
				rlc_parser_fail(parser, "COMMON intro forbids default case");
			hasDefault = 1;
		}

		if(case_stmt.fIsFallthrough && out->fCommonIntro)
			rlc_parser_fail(parser, "COMMON intro forbids fallthrough (limitation of bootstrap compiler)");

		rlc_parsed_switch_statement_add_case(
			out,
			&case_stmt);
	} while(case_stmt.fIsFallthrough
		|| !rlc_parser_consume(parser, NULL, kRlcTokBraceClose));

	rlc_parser_untrace(parser, &tracer);

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

void rlc_parsed_switch_statement_print(
	struct RlcParsedSwitchStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fIsVariableSwitchValue)
	{
		fputs("{\n\t", out);
		rlc_parsed_variable_print_argument(
			&this->fSwitchValue.fVariable,
			file,
			out,
			1);
		fputs(";\n\tswitch(", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(
				&this->fSwitchValue.fVariable,
				RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("){\n", out);
	} else
	{
		fputs("switch(__rl::mk_auto(", out);
		rlc_parsed_expression_print(this->fSwitchValue.fExpression, file, out);
		fputs(")){\n", out);
	}

	int hasDefault = 0;
	for(size_t i = 0; i < this->fCaseCount; i++)
	{
		if(this->fCases[i].fIsDefault)
			hasDefault = 1;

		rlc_parsed_case_statement_print(&this->fCases[i], this, file, out);
	}

	if(!hasDefault && this->fIsStrict)
	{
		struct RlcSrcString exp;
		if(this->fIsVariableSwitchValue)
			exp = RLC_BASE_CAST(&this->fSwitchValue.fVariable, RlcParsedScopeEntry)->fName;
		else
		{
			exp = this->fSwitchValue.fExpression->fStart.content;
			struct RlcSrcString end = this->fSwitchValue.fExpression->fEnd.content;
			exp.length = end.start + end.length - exp.start;
		}

		fprintf(out, "default: throw \"%s:%u:%u: value \"",
			file->fName,
			exp.line,
			exp.column);

		fputs(" __rl_assert_stringify_code(", out);
		rlc_src_string_print(&exp, file, out);
		fputs(") \" not covered in strict switch (consider SWITCH?(...))\";\n", out);
	}

	fputs("}\n", out);
	if(this->fIsVariableSwitchValue)
		fputs("}\n", out);
	rlc_parsed_control_label_print(&this->fLabel, file, out, "_break");
}