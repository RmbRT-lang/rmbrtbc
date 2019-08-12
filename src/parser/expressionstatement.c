#include "expressionstatement.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_expression_statement_create(
	struct RlcParsedExpressionStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedExpressionStatement);

	this->fExpression = NULL;
}

void rlc_parsed_expression_statement_destroy(
	struct RlcParsedExpressionStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fExpression);
		rlc_free((void**)&this->fExpression);
	}
	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_expression_statement_parse(
	struct RlcParsedExpressionStatement * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;

	rlc_parsed_expression_statement_create(
		out);

	out->fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType)
		&~RLC_FLAG(kRlcParsedTypeNameExpression));


	if(!out->fExpression)
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedExpression;
			goto failure;
		}

		goto nonfatal_failure;
	}
	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_expression_statement_destroy(out);
	return 0;
}