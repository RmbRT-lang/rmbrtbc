/** @file thisexpression.h
	Contains the 'this' expression type. */
#ifndef __rlc_scoper_thisexpression_h_defined
#define __rlc_scoper_thisexpression_h_defined
#pragma once

#include "expression.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedThisExpression;

#define kRlcScopedThisExpression kRlcParsedThisExpression

/** Scoped 'this' expression type. */
struct RlcScopedThisExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);
};

/** Creates a new scoped 'this' expression from a parsed 'this' expression.
@memberof RlcScopedThisExpression
@param[in] parsed:
	The parsed 'this' expression.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@return
	The scoped 'this' expression. */
struct RlcScopedThisExpression * rlc_scoped_this_expression_new(
	struct RlcParsedThisExpression const * parsed,
	struct RlcSrcFile const * file);

/** Destroys and deletes a scoped 'this' expression.
@memberof RlcScopedThisExpression
@param[in,out] this:
	The scoped 'this' expression to destroy.
	@dassert @nonnull */
void rlc_scoped_this_expression_delete(
	struct RlcScopedThisExpression * this);

#ifdef __cplusplus
}
#endif


#endif