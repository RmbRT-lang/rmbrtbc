#include "sizeofexpression.h"
#include "../assert.h"

void rlc_parsed_sizeof_expression_create(
	struct RlcParsedSizeofExpression * this,
	size_t start)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedSizeofExpression,
		start);

	rlc_parsed_type_name_create(&this->fType);
}

void rlc_parsed_sizeof_expression_destroy(
	struct RlcParsedSizeofExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_type_name_destroy(&this->fType);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_sizeof_expression_parse(
	struct RlcParsedSizeofExpression * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start = parser->fIndex;
	enum RlcParseError error_code;

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokSizeof))
		return 0;

	rlc_parsed_sizeof_expression_create(out, start);

	if(!rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		error_code = kRlcParseErrorExpectedParentheseOpen;
		goto failure;
	}

	if(!rlc_parsed_type_name_parse(
		&out->fType,
		parser))
	{
		error_code = kRlcParseErrorExpectedTypeName;
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
	rlc_parser_data_add_error(
		parser,
		error_code);
	parser->fIndex = start;
	rlc_parsed_sizeof_expression_destroy(out);
	return 0;
}
