/** @file stringexpression.h
	Contains the scoped string expression type. */
#ifndef __rlc_scoped_stringexpression_h_defined
#define __rlc_scoped_stringexpression_h_defined
#pragma once

#include "expression.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A scoped string expression.
@implements RlcScopedExpression */
struct RlcScopedStringExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);
	struct RlcScopedText fValue;
};

struct RlcParsedStringExpression;

/** Creates a new string expression from a parsed string expression.
@memberof RlcScopedExpression
@param[in] parsed:
	The parsed string expression.
	@dassert @nonnull
@param[in] file:
	The expression's source file.
	@dassert @nonnull
@return
	The new string expression. */
struct RlcScopedStringExpression * rlc_scoped_string_expression_new(
	struct RlcParsedStringExpression * parsed,
	struct RlcSrcFile * file);

/** Destroys and deletes a string expression.
@memberof RlcScopedStringExpression
@param[in,out] this:
	The string expression to delete.
	@dassert @nonnull */
void rlc_scoped_string_expression_delete(
	struct RlcScopedStringExpression * this);

#ifdef __cplusplus
}
#endif

#endif