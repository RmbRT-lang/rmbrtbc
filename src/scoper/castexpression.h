/** @file castexpression.h
	Contains the scoped cast expression type. */
#ifndef __rlc_scoper_cast_expression_h_defined
#define __rlc_scoper_cast_expression_h_defined

#include "expression.h"
#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcParsedCastExpression;

#define kRlcScopedCastExpression kRlcParsedCastExpression

/** Scoped cast expression type.
@extends RlcScopedExpression */
struct RlcScopedCastExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);

	/** The cast expression's type. */
	struct RlcScopedTypeName type;
	/** The cast expression's value. */
	struct RlcScopedExpression * value;
};


/** Creates a new scoped cast expression from a parsed cast expression.
@memberof RlcScopedCastExpression
@param[in] parsed:
	The parsed cast expression.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@return
	The scoped cast expression. */
struct RlcScopedCastExpression * rlc_scoped_cast_expression_new(
	struct RlcParsedCastExpression const * parsed,
	struct RlcSrcFile const * file);

/** Destroys and deletes a scoped cast expression.
@memberof RlcScopedCastExpression
@param[in,out] this:
	The scoped cast expression to delete.
	@dassert @nonnull */
void rlc_scoped_cast_expression_delete(
	struct RlcScopedCastExpression * this);


#ifdef __cplusplus
}
#endif

#endif