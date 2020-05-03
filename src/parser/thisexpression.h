/** @file thisexpression.h
	Contans the `this` expression type. */
#ifndef __rlc_parser_thisexpression_h_defined
#define __rlc_parser_thisexpression_h_defined

#include "expression.h"
#include "parser.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The `this` expression.
@implements RlcParsedExpression */
struct RlcParsedThisExpression
{
	RLC_DERIVE(struct, RlcParsedExpression);
};

/** Creates a `this` expression.
@memberof RlcParsedThisExpression
@param[out] this:
	The `this` expression to create.
	@dassert @nonnull
@param[in] first:
	The expression's first token. */
void rlc_parsed_this_expression_create(
	struct RlcParsedThisExpression * this,
	struct RlcSrcString const * string);

/** Destroys a `this` expression.
@memberof RlcParsedThisExpression
@param[in,out] this:
	The `this` expression to destroy.
	@dassert @nonnull */
void rlc_parsed_this_expression_destroy(
	struct RlcParsedThisExpression * this);

/** Parses a `this` expression.
@memberof RlcParsedThisExpression
@param[out] out:
	The `this` expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the `this` expression could be parsed. */
_Nodiscard int rlc_parsed_this_expression_parse(
	struct RlcParsedThisExpression * out,
	struct RlcParser * parser);

void rlc_parsed_this_expression_print(
	struct RlcParsedThisExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif