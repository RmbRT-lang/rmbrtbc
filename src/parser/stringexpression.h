/** @file stringexpression.h
	Contains the definition of the string expression type. */
#ifndef __rlc_parser_stringexpression_h_defined
#define __rlc_parser_stringexpression_h_defined

#include "../macros.h"
#include "expression.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif


/** A string expression.
	String expressions can consist of one or more string literals.
@implements RlcParsedExpression */
struct RlcParsedStringExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	/** The string literal(s). */
	struct RlcToken * fTokens;
	/** How many literals are chained together. */
	size_t fTokenCount;
};

/** Initialises a string expression.
@memberof RlcParsedStringExpression

@param[out] this:
	The string expression to initialise.
	@dassert @nonnull
@param[in] first:
	The expression's first token.
	@dassert @nonnull */
void rlc_parsed_string_expression_create(
	struct RlcParsedStringExpression * this,
	struct RlcToken const * first);

/** Destroys a strin expression.
@memberof RlcParsedStringExpression

@param[in,out] this:
	The string expression to destroy.
	@dassert @nonnull */
void rlc_parsed_string_expression_destroy(
	struct RlcParsedStringExpression * this);

/** Parses a string expression.
@memberof RlcParsedStringExpression

@param[out] out:
	The string expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_string_expression_parse(
	struct RlcParsedStringExpression * out,
	struct RlcParser * parser);

void rlc_parsed_string_expression_print(
	struct RlcParsedStringExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif