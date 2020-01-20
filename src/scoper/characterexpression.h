/** @file characterexpression.h
	Contains the scoped character expression type. */
#ifndef __rlc_scoped_characterexpression_h_defined
#define __rlc_scoped_characterexpression_h_defined
#pragma once

#include "expression.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A scoped character expression.
@implements RlcScopedExpression */
struct RlcScopedCharacterExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);
	struct RlcScopedText fValue;
};

struct RlcParsedCharacterExpression;

/** Creates a new character expression from a parsed character expression.
@memberof RlcScopedExpression
@param[in] parsed:
	The parsed character expression.
	@dassert @nonnull
@param[in] file:
	The expression's source file.
	@dassert @nonnull
@return
	The new character expression. */
struct RlcScopedCharacterExpression * rlc_scoped_character_expression_new(
	struct RlcParsedCharacterExpression * parsed,
	struct RlcSrcFile * file);

/** Destroys and deletes a character expression.
@memberof RlcScopedCharacterExpression
@param[in,out] this:
	The character expression to delete.
	@dassert @nonnull */
void rlc_scoped_character_expression_delete(
	struct RlcScopedCharacterExpression * this);

#ifdef __cplusplus
}
#endif

#endif