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

	this->fHasLabel = 0;
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

static int parse_initial(
	struct RlcParsedLoopStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;

	if(!(out->fIsVariableInitial = rlc_parsed_variable_parse(
		&out->fInitial.fVariable,
		parser,
		1,
		1,
		0)))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedVariable;
			goto failure;
		}
	} else goto success;
	if(!(out->fInitial.fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedExpression;
			goto failure;
		}
	} else goto success;
success:
	return 1;
failure:
	rlc_parser_data_add_error(parser, error_code);
	return 0;
}

static int parse_for_head(
	struct RlcParsedLoopStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokFor))
		return 0;

	enum RlcParseError error_code;

	if(!out->fIsPostCondition)
	{
		if(out->fHasLabel = rlc_parser_data_consume(
			parser,
			kRlcTokIdentifier))
		{
			out->fLabelToken = rlc_parser_data_consumed_index(parser);
			if(!rlc_parser_data_consume(
				parser,
				kRlcTokColon))
			{
				error_code = kRlcParseErrorExpectedColon;
				goto failure;
			}
		}
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		error_code = kRlcParseErrorExpectedParentheseOpen;
		goto failure;
	}

	if(!out->fIsPostCondition)
	{
		if(!parse_initial(out, parser))
		{
			error_code = kRlcParseErrorExpectedExpressionOrVariable;
			goto failure;
		}
		if(!rlc_parser_data_consume(
			parser,
			kRlcTokSemicolon))
		{
			error_code = kRlcParseErrorExpectedSemicolon;
			goto failure;
		}
	}

	out->fIsVariableCondition = 0;

	if(!(out->fCondition.fExpression =
		rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType)))
	&& parser->fErrorCount)
	{
		error_code = kRlcParseErrorExpectedExpression;
		goto failure;
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

	if(!(out->fPostLoop =
		rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType)))
	&& parser->fErrorCount)
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

	return 1;
failure:
	rlc_parser_data_add_error(parser, error_code);
	return 0;
}

static int parse_while_head(
	struct RlcParsedLoopStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokWhile))
		return 0;

	enum RlcParseError error_code;

	if(!out->fIsPostCondition)
	{
		if(out->fHasLabel = rlc_parser_data_consume(
			parser,
			kRlcTokIdentifier))
		{
			out->fLabelToken = rlc_parser_data_consumed_index(parser);
			if(!rlc_parser_data_consume(
				parser,
				kRlcTokColon))
			{
				error_code = kRlcParseErrorExpectedColon;
				goto failure;
			}
		}
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		error_code = kRlcParseErrorExpectedParentheseOpen;
		goto failure;
	}


	if(!(out->fIsVariableCondition =
		rlc_parsed_variable_parse(
			&out->fCondition.fVariable,
			parser,
			1,
			1,
			1)))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedVariable;
			goto failure;
		}
		if(!(out->fCondition.fExpression =
			rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType))))
		{
			error_code = parser->fErrorCount
				? kRlcParseErrorExpectedExpression
				: kRlcParseErrorExpectedExpressionOrVariable;
			goto failure;
		}
	}

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseClose))
	{
		error_code = kRlcParseErrorExpectedParentheseClose;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(parser, error_code);
	return 0;
}

int rlc_parsed_loop_statement_parse(
	struct RlcParsedLoopStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;

	if(!rlc_parser_data_match(
		parser, kRlcTokDo)
	&& !rlc_parser_data_match(
		parser, kRlcTokFor)
	&& !rlc_parser_data_match(
		parser, kRlcTokWhile))
		return 0;

	rlc_parsed_loop_statement_create(out);

	if(out->fIsPostCondition = rlc_parser_data_consume(
		parser,
		kRlcTokDo))
	{
		if(rlc_parser_data_match_ahead(
			parser,
			kRlcTokColon)
		&& (out->fHasLabel = rlc_parser_data_consume(
			parser,
			kRlcTokIdentifier)))
		{
			out->fLabelToken = rlc_parser_data_consumed_index(parser);
			rlc_parser_data_next(parser);
		}

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokParentheseOpen))
		{
			error_code = kRlcParseErrorExpectedParentheseOpen;
			goto parsed_head;
		}

		if(!parse_initial(out, parser))
		{
			error_code = kRlcParseErrorExpectedLoopInitial;
			goto failure;
		}

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokParentheseClose))
		{
			error_code = kRlcParseErrorExpectedParentheseClose;
			goto failure;
		}
	} else
	{
		if(!parse_for_head(out, parser))
		{
			if(parser->fErrorCount)
			{
				error_code = kRlcParseErrorExpectedForHead;
				goto failure;
			}
		} else goto parsed_head;
		if(!parse_while_head(out, parser))
		{
			if(parser->fErrorCount)
			{
				error_code = kRlcParseErrorExpectedWhileHead;
				goto failure;
			} else
			{
				error_code = kRlcParseErrorExpectedLoopHead;
				goto failure;
			}
		}
	}
parsed_head:
	if(!(out->fBody = rlc_parsed_statement_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedStatementType))))
	{
		error_code = kRlcParseErrorExpectedStatement;
		goto failure;
	}

	if(out->fIsPostCondition)
	{
		if(!parse_for_head(out, parser))
		{
			if(parser->fErrorCount)
			{
				error_code = kRlcParseErrorExpectedForHead;
				goto failure;
			}
		} else goto success;

		if(!parse_while_head(out, parser))
		{
			if(parser->fErrorCount)
			{
				error_code = kRlcParseErrorExpectedForHead;
				goto failure;
			} else
			{
				error_code = kRlcParseErrorExpectedLoopHead;
				goto failure;
			}
		} else goto success;
	}

success:
	return 1;

failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_loop_statement_destroy(out);
	return 0;
}