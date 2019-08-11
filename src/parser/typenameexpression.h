/** @file typenameexpression.h
	Contains the type name expression type. */
#ifndef __rlc_parser_expression_typenameexpression_h_defined
#define __rlc_parser_expression_typenameexpression_h_defined

#include "expression.h"
#include "typename.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A type name expression.
@extends RlcParsedExpression
@extends RlcParsedTypeName */
struct RlcParsedTypeNameExpression
{
	RLC_DERIVE(struct, RlcParsedExpression);
	RLC_DERIVE(struct, RlcParsedTypeName);
};

/** Creates a type name expression.
@param[out] this:
	The type name expression to create.
	@dassert @nonnull
@param[in] first:
	The expression's first token. */
void rlc_parsed_type_name_expression_create(
	struct RlcParsedTypeNameExpression * this,
	size_t first);

/** Destroys a parsed type name expression.
@param[in,out] this:
	The type name expression to destroy.
	@dassert @nonnull */
void rlc_parsed_type_name_expression_destroy(
	struct RlcParsedTypeNameExpression * this);

/** Parses a type name expression.
@memberof RlcParsedTypeNameExpression
@param[out] out:
	The type name expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull */
int rlc_parsed_type_name_expression_parse(
	struct RlcParsedTypeNameExpression * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif


#endif