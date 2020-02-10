/** @file expression.h
	Contains the scoped expression type. */
#ifndef __rlc_scoper_expression_h_defined
#define __rlc_scoper_expression_h_defined
#pragma once

#include "../macros.h"
#include "../parser/expression.h"

#ifdef __cplusplus
extern "C" {
#endif

// Since there is a 1:1 mapping from parsed to scoped expression, use the same enum.
#define RlcScopedExpressionType RlcParsedExpressionType

/** Scoped expression. */
struct RlcScopedExpression
{
	RLC_ABSTRACT(RlcScopedExpression);
	/** The parsed version of the expression. */
	struct RlcParsedExpression const * fParsed;
};

/** Creates a scoped expression from a parsed expression.
@memberof RlcScopedExpression
@param[in] parsed:
	The parsed expression.
	@dassert @nonnull
@param[in] file:
	The source file containing the expression.
	@dassert @nonnull
@return
	The scoped expression. */
struct RlcScopedExpression * rlc_scoped_expression_new(
	struct RlcParsedExpression const * parsed,
	struct RlcSrcFile const * file);

/** Destroys and deletes a scoped expression.
@memberof RlcScopedExpression
@param[in,out] this:
	The scoped expression to destroy and delete.
	@dassert @nonnull */
void rlc_scoped_expression_delete_virtual(
	struct RlcScopedExpression * this);

/** Creates a scoped expression from a parsed expression.
@memberof RlcScopedExpression
@param[out] this:
	The scoped expression to create.
	@dassert @nonnull
@param[in] parsed:
	The expression's parsed version.
	@dassert @nonnull
@param[in] type:
	The expression's deriving type. */
void rlc_scoped_expression_create(
	struct RlcScopedExpression * this,
	struct RlcParsedExpression const * parsed,
	enum RlcScopedExpressionType type);

/** Destroys a scoped expression's base.
@memberof RlcScopedExpression
@param[in,out] this:
	The scoped expression to destroy.
	@dassert @nonnull. */
void rlc_scoped_expression_destroy_base(
	struct RlcScopedExpression * this);

#ifdef __cplusplus
}
#endif

#endif