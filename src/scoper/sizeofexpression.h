/** @file sizeofexpression.h
	Contains the scoped sizeof expression type. */
#ifndef __rlc_scoper_sizeofexpression_h_defined
#define __rlc_scoper_sizeofexpression_h_defined
#pragma once

#include "expression.h"
#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedSizeofExpression;

#define kRlcScopedSizeofExpression kRlcParsedSizeofExpression

/** Scoped sizeof expression type.
@extends RlcScopedExpression */
struct RlcScopedSizeofExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);

	/** The type whose size is to be retrieved. */
	struct RlcScopedTypeName type;
};

/** Creates a new scoped sizeof expression from a parsed sizeof expression.
@memberof RlcScopedSizeofExpression
@param[in] parsed:
	The parsed sizeof expression.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@return
	The new scoped sizeof expression.
	@dassert @nonnull */
struct RlcScopedSizeofExpression * rlc_scoped_sizeof_expression_new(
	struct RlcParsedSizeofExpression const * parsed,
	struct RlcSrcFile const * file);

/** Destroys and deletes a scoped sizeof expression.
@memberof RlcScopedSizeofExpression
@param[in,out] this:
	the scoped sizeof expression to destroy.
	@dassert @nonnull */
void rlc_scoped_sizeof_expression_delete(
	struct RlcScopedSizeofExpression * this);

#ifdef __cplusplus
}
#endif

#endif