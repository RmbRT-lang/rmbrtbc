#include "throwstatement.h"

#include "../malloc.h"

void rlc_parsed_throw_statement_create(
	struct RlcParsedThrowStatement * this)
{
	RLC_DASSERT(this != NULL);

	this->fType = kRlcThrowTypeRethrow;

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedThrowStatement);
}

void rlc_parsed_throw_statement_destroy(
	struct RlcParsedThrowStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fType == kRlcThrowTypeValue)
	{
		rlc_parsed_expression_destroy_virtual(this->fValue);
		rlc_free((void**)&this->fValue);
		this->fType = kRlcThrowTypeRethrow;
	}

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_throw_statement_parse(
	struct RlcParsedThrowStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokThrow))
		return 0;

	rlc_parsed_throw_statement_create(out);

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokTripleDot))
	{
		out->fType = kRlcThrowTypeRethrow;
		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokSemicolon);
		return 1;
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokSemicolon))
	{
		out->fType = kRlcThrowTypeVoid;
		return 1;
	}

	out->fType = kRlcThrowTypeValue;
	if(!(out->fValue = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
		rlc_parser_fail(parser, "expected expression");

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);
	return 1;
}