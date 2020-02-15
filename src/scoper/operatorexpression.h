/** @file operatorexpression.h
	Contains the operator expression type used by the scoper. */
#ifndef __rlc_scoper_operatorexpression_h_defined
#define __rlc_scoper_operatorexpression_h_defined
#pragma once

#include "expression.h"
#include "../parser/operatorexpression.h"

#ifdef __cplusplus
extern "C" {
#endif

#define kRlcScopedOperatorExpression kRlcParsedOperatorExpression

/** Operator expression.
@extends RlcScopedExpression */
struct RlcScopedOperatorExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);
	struct RlcScopedExpression ** arguments;
};

/** Creates a scoped operator expression from a parsed one.
@memberof RlcScopedOperatorExpression
@param[out] this:
	The scoped operator expression to create.
	@dassert @nonnull
@param[in] parsed:
	The parsed operator expression.
	@dassert @nonnull
@param[in] file:
	The parsed expression's source file.
	@dassert @nonnull. */
void rlc_scoped_operator_expression_create(
	struct RlcScopedOperatorExpression * this,
	struct RlcParsedOperatorExpression const * parsed,
	struct RlcSrcFile const * file);

void rlc_scoped_operator_expression_destroy(
	struct RlcScopedOperatorExpression * this);

/** Retrieves a scoped operator expression's operator.
@memberof RlcScopedOperatorExpression
@param[in] this:
	The operator expression.
	@dassert @nonnull
@return
	The expression's operator. */
enum RlcOperator rlc_scoped_operator_expression_operator(
	struct RlcScopedOperatorExpression const * this);

RlcSrcSize rlc_scoped_operator_expression_argument_count(
	struct RlcScopedOperatorExpression const * this);

#ifdef __cplusplus
}
#endif

#endif