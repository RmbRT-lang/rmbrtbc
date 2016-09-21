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