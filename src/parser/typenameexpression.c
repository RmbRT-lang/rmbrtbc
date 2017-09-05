#include "typenameexpression.h"
#include "../assert.h"

void rlc_parsed_type_name_expression_create(
	struct RlcParsedTypeNameExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedTypeNameExpression);

	rlc_parsed_type_name_create(
		RLC_BASE_CAST(this, RlcParsedTypeName));
}

void rlc_parsed_type_name_expression_destroy(
	struct RlcParsedTypeNameExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_DERIVING_TYPE(RLC_BASE_CAST(this,RlcParsedExpression)) == kRlcParsedTypeNameExpression);

	rlc_parsed_type_name_destroy(
		RLC_BASE_CAST(this, RlcParsedTypeName));
}

int rlc_parsed_type_name_expression_parse(
	struct RlcParsedTypeNameExpression * out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_parsed_type_name_expression_create(out);

	enum RlcParseError error_code;

	if(!rlc_parsed_type_name_parse(
		RLC_BASE_CAST(out, RlcParsedTypeName),
		parser))
	{
		if(parser->fErrorCount)
		{
			error_code = kRlcParseErrorExpectedTypeName;
			goto failure;
		}
		else
			goto nonfatal_failure;
	}

success:
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_type_name_expression_destroy(out);
	return 0;
}