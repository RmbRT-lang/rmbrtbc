/** @file castexpression.h
	Contains the type-cast expression type as used by the parser. */
#ifndef __rlc_parser_castexpression_h_defined
#define __rlc_parser_castexpression_h_defined

#include "expression.h"
#include "typename.h"
#include "parser.h"

#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum RlcCastType
{
	kRlcCastTypeStatic,
	kRlcCastTypeDynamic,
	kRlcCastTypeConcept
};

/** A cast expression as used by the parser. */
struct RlcParsedCastExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	enum RlcCastType fMethod;

	/** The cast expression's type. */
	struct RlcParsedTypeName fType;
	/** The cast expression's values. */
	struct RlcParsedExpression ** fValues;
	RlcSrcSize fValueCount;
};

/** Creates a cast expression.
@memberof RlcParsedCastExpression
@param[out] this:
	The cast expression to create.
	@dassert @nonnull
@param[in] first:
	The expression's first token. */
void rlc_parsed_cast_expression_create(
	struct RlcParsedCastExpression * this,
	struct RlcToken first,
	struct RlcToken last);

/** Destroys a cast expression.
@memberof RlcParsedCastExpression
@param[in,out] this:
	The cast expression to destroy.
	@dassert @nonnull */
void rlc_parsed_cast_expression_destroy(
	struct RlcParsedCastExpression * this);

/** Parses a cast expression.
@memberof RlcParsedCastExpression
@param[out] out:
	The cast expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the expression was successfully parsed. */
int rlc_parsed_cast_expression_parse(
	struct RlcParsedCastExpression * out,
	struct RlcParser * parser);

void rlc_parsed_cast_expression_print(
	struct RlcParsedCastExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif