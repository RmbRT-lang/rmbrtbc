/** @file numberexpression.h
	Contains the definition of the numerical expression type. */
#ifndef __rlc_parser_numberexpression_h_defined
#define __rlc_parser_numberexpression_h_defined

#include "expression.h"
#include "parser.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A numerical expression.
	Numerical expressions can either be numbers or single characters.
@implements RlcParsedExpression */
struct RlcParsedNumberExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	/** The number token's index. */
	struct RlcToken fNumberToken;
};

/** Initialises a number expression.
@memberof RlcParsedNumberExpression

@param[out] this:
	The number expression to initialise.
@param[in] token:
	The number token. */
void rlc_parsed_number_expression_create(
	struct RlcParsedNumberExpression * this,
	struct RlcToken const * token);

/** Destroys a number expression.
@memberof RlcParsedNumberExpression

@param[in,out] this:
	The number expression to destroy. */
void rlc_parsed_number_expression_destroy(
	struct RlcParsedNumberExpression * this);

/** Parses a number expression.
@memberof RlcParsedNumberExpression
@param[out] out:
	The number expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the number expression could be parsed. */
_Nodiscard int rlc_parsed_number_expression_parse(
	struct RlcParsedNumberExpression * out,
	struct RlcParser * parser);

void rlc_parsed_number_expression_print(
	struct RlcParsedNumberExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif