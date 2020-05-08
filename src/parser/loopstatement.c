#include "loopstatement.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_loop_statement_create(
	struct RlcParsedLoopStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedLoopStatement);

	this->fIsVariableInitial = 0;
	this->fIsVariableCondition = 0;
	this->fCondition.fExpression = NULL;
	this->fInitial.fExpression = NULL;
	this->fBody = NULL;
	this->fPostLoop = NULL;
}

void rlc_parsed_loop_statement_destroy(
	struct RlcParsedLoopStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this,RlcParsedStatement));

	if(this->fIsVariableInitial)
	{
		rlc_parsed_variable_destroy(&this->fInitial.fVariable);
	} else if(this->fInitial.fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fInitial.fExpression);
		rlc_free((void**)&this->fInitial.fExpression);
	}

	if(this->fIsVariableCondition)
	{
		rlc_parsed_variable_destroy(&this->fCondition.fVariable);
	} else if(this->fCondition.fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fCondition.fExpression);
		rlc_free((void**)&this->fCondition.fExpression);
	}

	if(this->fBody)
	{
		rlc_parsed_statement_destroy_virtual(this->fBody);
		rlc_free((void**)&this->fBody);
	}

	if(this->fPostLoop)
	{
		rlc_parsed_expression_destroy_virtual(this->fPostLoop);
		rlc_free((void**)&this->fPostLoop);
	}
}

static void parse_initial(
	struct RlcParsedLoopStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!(out->fIsVariableInitial = rlc_parsed_variable_parse(
		&out->fInitial.fVariable,
		parser,
		NULL,
		1,
		1,
		0,
		0,
		0)))
	{
		out->fInitial.fExpression = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType));
	}
}

static _Nodiscard int parse_for_head(
	struct RlcParsedLoopStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokFor))
		return 0;

	if(!out->fIsPostCondition)
		rlc_parsed_control_label_parse(
			&out->fLabel,
			parser);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(!out->fIsPostCondition)
	{
		parse_initial(out, parser);
		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokSemicolon);
	}

	out->fIsVariableCondition = 0;
	out->fCondition.fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	out->fPostLoop = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);

	return 1;
}

static _Nodiscard int parse_while_head(
	struct RlcParsedLoopStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokWhile))
		return 0;

	if(!out->fIsPostCondition)
		rlc_parsed_control_label_parse(
			&out->fLabel,
			parser);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(!out->fIsPostCondition
	&& (out->fIsVariableCondition =
		rlc_parsed_variable_parse(
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
	} else if((out->fCondition.fExpression =
		rlc_parsed_expression_parse(
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

	return 1;
}

static _Nodiscard int parse_do_head(
	struct RlcParsedLoopStatement * out,
	struct RlcParser * parser)
{
	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokDo))
		return 0;

	out->fIsPostCondition = 1;
	rlc_parsed_control_label_parse(
		&out->fLabel,
		parser);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	parse_initial(out, parser);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);
	return 1;
}

static void parse_loop_head(
	struct RlcParsedLoopStatement * out,
	struct RlcParser * parser)
{
	out->fIsPostCondition = 0;
	if(!parse_do_head(out, parser)
	&& !parse_for_head(out, parser)
	&& !parse_while_head(out, parser))
	{
		rlc_parser_fail(parser, "expected loop head");
	}
}

int rlc_parsed_loop_statement_parse(
	struct RlcParsedLoopStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_is_current(
		parser, kRlcTokDo)
	&& !rlc_parser_is_current(
		parser, kRlcTokFor)
	&& !rlc_parser_is_current(
		parser, kRlcTokWhile))
		return 0;

	rlc_parsed_loop_statement_create(out);

	parse_loop_head(out, parser);

	if(!(out->fBody = rlc_parsed_statement_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedStatementType)
		&~RLC_FLAG(kRlcParsedVariableStatement))))
	{
		rlc_parser_fail(parser, "expected loop body");
	}

	if(out->fIsPostCondition)
	{
		if(!parse_for_head(out, parser)
		&& !parse_while_head(out, parser))
		{
			rlc_parser_fail(parser, "expected loop head");
		}
	}

	return 1;
}

void rlc_parsed_loop_statement_print(
	struct RlcParsedLoopStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fIsVariableInitial)
	{
		fputs("{", out);
		rlc_parsed_variable_print_argument(&this->fInitial.fVariable, file, out, 1);
		fputs(";\n", out);
	} else
	{
		if(this->fInitial.fExpression)
		{
			rlc_parsed_expression_print(this->fInitial.fExpression, file, out);
			fputs(";\n", out);
		}
	}

	if(this->fIsPostCondition)
	{
		/*
		goto skip_first;
		while(condition)
		{
			f();
		skip_first:
			... body
		} */
		fputs("__rl_do_while_loop(", out);
		if(this->fPostLoop)
		{
			RLC_DASSERT(!this->fIsVariableCondition);
			rlc_parsed_expression_print(this->fPostLoop, file, out);
		}
		fputs(")", out);

		rlc_parsed_statement_print(this->fBody, file, out);

		rlc_parsed_control_label_print(
			&this->fLabel,
			file,
			out,
			"_continue");
		fputs("} while(", out);
		if(this->fIsVariableCondition)
			rlc_parsed_variable_print_argument(
				&this->fCondition.fVariable,
				file,
				out,
				1);
		else if(this->fCondition.fExpression)
			rlc_parsed_expression_print(this->fCondition.fExpression, file, out);
		else
			fputs("true", out);

		fputs(");\n", out);
		rlc_parsed_control_label_print(
			&this->fLabel,
			file,
			out,
			"_break");
	} else
	{
		fputs("for(;", out);
		if(this->fIsVariableCondition)
			rlc_parsed_variable_print_argument(
				&this->fCondition.fVariable,
				file,
				out,
				1);
		else if(this->fCondition.fExpression)
			rlc_parsed_expression_print(this->fCondition.fExpression, file, out);

		fputs("; ", out);

		if(this->fPostLoop)
			rlc_parsed_expression_print(this->fPostLoop, file, out);
		fputs(")\n{\n\t", out);
		rlc_parsed_statement_print(this->fBody, file, out);
		rlc_parsed_control_label_print(
			&this->fLabel,
			file,
			out,
			"_continue");
		fputs("{;}\n}\n", out);
	}

	rlc_parsed_control_label_print(&this->fLabel, file, out, "_break");

	if(this->fIsVariableInitial)
		fputs("}\n", out);
}