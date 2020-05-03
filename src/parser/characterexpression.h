/** @file characterexpression.h
	Contains the character expression type as used by the parser. */
#ifndef __rlc_parser_characterexpression_h_defined
#define __rlc_parser_characterexpression_h_defined
#pragma once

#include "expression.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Parsed character literal expression.
@implements RlcParsedExpression */
struct RlcParsedCharacterExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	/** The character literal token. */
	struct RlcToken fToken;
};

/** Creates a parsed character expression.
@memberof RlcParsedCharacterExpression
@param[out] this:
	The character expression to create.
	@dassert @nonnull
@param[in] token:
	The expression's character literal.
	@dassert
		@nonnull
		The token must be a character literal. */
void rlc_parsed_character_expression_create(
	struct RlcParsedCharacterExpression * this,
	struct RlcToken const * token);

/** Destroys a parsed character expression.
@memberof RlcParsedCharacterExpression
@param[in,out] this:
	The character expression to destroy.
	@dassert @nonnull */
void rlc_parsed_character_expression_destroy(
	struct RlcParsedCharacterExpression * this);

/** Parses a parsed character expression.
@memberof RlcParsedCharacterExpression
@param[out] out:
	The character expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser.
	@dassert @nonnull
@return
	Whether the character expression was successfully parsed. */
_Nodiscard int rlc_parsed_character_expression_parse(
	struct RlcParsedCharacterExpression * out,
	struct RlcParser * parser);


void rlc_parsed_character_expression_print(
	struct RlcParsedCharacterExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif


#endif