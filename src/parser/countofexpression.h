/** @file countofexpression.h
	Contains the countof expression type as used by the parser. */
#ifndef __rlc_parser_countofexpression_h_defined
#define __rlc_parser_countofexpression_h_defined

#include "expression.h"
#include "typename.h"
#include "../macros.h"


#ifdef __cplusplus
extern "C" {
#endif

/** A countof expression as used by the parser. */
struct RlcParsedCountofExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	struct RlcParsedSymbol fType;
};

/** Creates a countof expression.
@memberof RlcParsedCountofExpression
@param[out] this:
	The countof expression to create.
	@dassert @nonull
@param[in] start:
	The expression's first token. */
void rlc_parsed_countof_expression_create(
	struct RlcParsedCountofExpression * this,
	struct RlcToken start,
	struct RlcToken end);

/** Destroys a countof expression.
@memberof RlcParsedCountofExpression
@param[in,out] this:
	The countof expression to destroy.
	@dassert @nonull */
void rlc_parsed_countof_expression_destroy(
	struct RlcParsedCountofExpression * this);

/** Parses a countof expression.
@memberof RlcParsedCountofExpression
@param[out] out:
	The countof expression to parse.
	@dassert @nonnull
@param[in,out] paraser:
	The parser data.
	@dassert @nonnull
@return
	Whether the expression was successfully parsed. */
_Nodiscard int rlc_parsed_countof_expression_parse(
	struct RlcParsedCountofExpression * out,
	struct RlcParser * parser);

void rlc_parsed_countof_expression_print(
	struct RlcParsedCountofExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif


#endif