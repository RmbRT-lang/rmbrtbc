/** @file nullexpression.h
	Contans the `NULL` expression type. */
#ifndef __rlc_parser_nullexpression_h_defined
#define __rlc_parser_nullexpression_h_defined

#include "expression.h"
#include "parser.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The `this` expression.
@implements RlcParsedExpression */
struct RlcParsedNullExpression
{
	RLC_DERIVE(struct, RlcParsedExpression);
	int fNullOrBare;
};

/** Creates a `this` expression.
@memberof RlcParsedNullExpression
@param[out] this:
	The `this` expression to create.
	@dassert @nonnull
@param[in] first:
	The expression's first token. */
void rlc_parsed_null_expression_create(
	struct RlcParsedNullExpression * this,
	struct RlcToken token);

/** Destroys a `this` expression.
@memberof RlcParsedNullExpression
@param[in,out] this:
	The `this` expression to destroy.
	@dassert @nonnull */
void rlc_parsed_null_expression_destroy(
	struct RlcParsedNullExpression * this);

/** Parses a `this` expression.
@memberof RlcParsedNullExpression
@param[out] out:
	The `this` expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the `this` expression could be parsed. */
_Nodiscard int rlc_parsed_null_expression_parse(
	struct RlcParsedNullExpression * out,
	struct RlcParser * parser);

void rlc_parsed_null_expression_print(
	struct RlcParsedNullExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif