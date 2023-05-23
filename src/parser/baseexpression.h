/** @file baseexpression.h
	Contains the definition of the base-access expression used by the parser. */
#ifndef __rlc_parser_baseexpression_h_defined
#define __rlc_parser_baseexpression_h_defined

#include "symbol.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A base-access expression.
@implements RlcParsedExpression */
struct RlcParsedBaseExpression
{
	RLC_DERIVE(struct, RlcParsedExpression);

	int fIsName;

	union {
		struct RlcToken fIndex;
		struct RlcParsedSymbol fName;
	} fBase;
};

/** Initialises a base expression.
@memberof RlcParsedBaseExpression
@param[out] this:
	The base expression to initialise. */
void rlc_parsed_base_expression_create(
	struct RlcParsedBaseExpression * this,
	struct RlcToken start,
	struct RlcToken end);

/** Destroys a base expression.
@memberof RlcParsedBaseExpression

@param[in,out] this:
	The base expression to destroy. */
void rlc_parsed_base_expression_destroy(
	struct RlcParsedBaseExpression * this);

/** Parses a base expression.
@memberof RlcParsedBaseExpression
@param[out] out:
	The base expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the base expression could be parsed. */
_Nodiscard int rlc_parsed_base_expression_parse(
	struct RlcParsedBaseExpression * out,
	struct RlcParser * parser);


void rlc_parsed_base_expression_print(
	struct RlcParsedBaseExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif