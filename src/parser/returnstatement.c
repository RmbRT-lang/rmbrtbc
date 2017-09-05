#include "returnstatement.h"
#include "../assert.h"
#include "../malloc.h"
#include "../tokeniser/tokens.h"

void rlc_parsed_return_statement_create(
	struct RlcParsedReturnStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedReturnStatement);
	this->fExpression = NULL;
}

void rlc_parsed_return_statement_destroy(
	struct RlcParsedReturnStatement * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this,RlcParsedStatement)) == kRlcParsedReturnStatement);

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));

	if(this->fExpression)
	{
		rlc_parsed_expression_destroy_virtual(this->fExpression);
		rlc_free((void**)&this->fExpression);
	}
}

int rlc_parsed_return_statement_parse(
	struct RlcParsedReturnStatement * out,
	struct RlcParserData * parser)
{
	if(!rlc_parser_data_consume(
		parser,
		kRlcTokReturn))
		return 0;

	enum RlcParseError error_code;

	rlc_parsed_return_statement_create(out);
	if(!(out->fExpression = rlc_parsed_expression_parse(
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

	return 1;

failure:
	rlc_parser_data_add_error(parser, error_code);
	rlc_parsed_return_statement_destroy(out);
	return 0;
}