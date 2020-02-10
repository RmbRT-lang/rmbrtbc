/** @file numberexpression.h
	Contains the scoped number expression type. */
#ifndef __rlc_scoper_numberexpression_h_defined
#define __rlc_scoper_numberexpression_h_defined

#include "../macros.h"
#include "../parser/numberexpression.h"
#include "expression.h"
#include "number.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Number expression.
@implements RlcScopedExpression */
struct RlcScopedNumberExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);

	struct RlcNumber value;
};

/** Creates a scoped number expression from a parsed number expression.
@memberof RlcScopedExpression
@param[out] this:
	The scoped number expression to create.
	@dassert @nonnull
@param[in] parsed:
	The parsed number expression.
	@dassert @nonnull
@param[in] file:
	The parsed number expression's source file.
	@dassert @nonnull */
void rlc_scoped_number_expression_create(
	struct RlcScopedNumberExpression * this,
	struct RlcParsedNumberExpression const * parsed,
	struct RlcSrcFile const * file);

/** Destroys a scoped number expression.
@memberof RlcScopedNumberExpression
@param[in,out] this:
	The scoped number expression to destroy.
	@dassert @nonnull */
void rlc_scoped_number_expression_destroy(
	struct RlcScopedNumberExpression * this);

#ifdef __cplusplus
}
#endif

#endif