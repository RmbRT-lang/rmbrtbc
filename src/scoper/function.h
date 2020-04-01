/** @file function.h
	Contains the scoped function type. */
#ifndef __rlc_scoper_function_h_defined
#define __rlc_scoper_function_h_defined
#pragma once

#include "typename.h"
#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedFunction;
struct RlcScopedStatement;
struct RlcScopedExpression;
struct RlcScopedVariable;

/** Function class as used by the scoper.
@extends RlcScopedScopeEntry */
struct RlcScopedFunction
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);

	/** The scoped function's arguments. */
	struct RlcScopedVariable ** arguments;
	/** The function's return type, if any. */
	struct RlcScopedTypeName returnType;
	/** The function's body, if it has one. */
	union
	{
		struct RlcScopedStatement * statement;
		struct RlcScopedExpression * expression;
	} body;
};

/** Creates a scoped function from a parsed function.
@memberof RlcScopedFunction
@param[out] this:
	The scoped function.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed function.
	@dassert @nonnull
@param[in] parent:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_function_create(
	struct RlcScopedFunction * this,
	struct RlcSrcFile const * file,
	struct RlcParsedFunction const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped function.
@memberof RlcScopedFunction
@param[in,out] this:
	The function to destroy.
	@dassert @nonnull */
void rlc_scoped_function_destroy(
	struct RlcScopedFunction * this);


#ifdef __cplusplus
}
#endif


#endif