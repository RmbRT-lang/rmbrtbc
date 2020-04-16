/** @file function.h
	Contains the scoped function type. */
#ifndef __rlc_scoper_function_h_defined
#define __rlc_scoper_function_h_defined
#pragma once

#include "typename.h"
#include "scopeentry.h"
#include "member.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedFunction;
struct RlcParsedMemberFunction;
struct RlcScopedStatement;
struct RlcScopedExpression;
struct RlcScopedVariable;

/** Function class as used by the scoper. */
struct RlcScopedFunction
{
	/** The scoped function's argument count. */
	RlcSrcSize argumentCount;
	/** Whether the function has an explicit return type. */
	char hasReturnType;
	/** Whether the function has a body. */
	char hasBody;
	/** Whether the function's body is a short-hand return expression. */
	char isShortHandBody;
	/** The scoped function's arguments. */
	struct RlcScopedGlobalVariable ** arguments;
	/** The function's return type, if any. */
	struct RlcScopedTypeName returnType;
	/** The function's body, if it has one. */
	union
	{
		struct RlcScopedStatement * statement;
		struct RlcScopedExpression * expression;
	} body;
};

/** Global function class used by the scoper.
@extends RlcScopedScopeEntry
@extends RlcScopedFunction */
struct RlcScopedGlobalFunction
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
	RLC_DERIVE(struct,RlcScopedFunction);
};

/** Member function class used by the scoper.
@extends RlcScopedMember
@extends RlcScopedFunction */
struct RlcScopedMemberFunction
{
	RLC_DERIVE(struct,RlcScopedMember);
	RLC_DERIVE(struct,RlcScopedFunction);
};

/** Creates a scoped global function from a parsed global function.
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
void rlc_scoped_global_function_create(
	struct RlcScopedGlobalFunction * this,
	struct RlcSrcFile const * file,
	struct RlcParsedFunction const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped global function.
@memberof RlcScopedFunction
@param[in,out] this:
	The function to destroy.
	@dassert @nonnull */
void rlc_scoped_global_function_destroy(
	struct RlcScopedGlobalFunction * this);

/** Creates a scoped member function from a parsed member function.
@memberof RlcScopedMemberFunction
@param[out] this:
	The scoped member function to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member function.
	@dassert @nonnull
@param[in,out] scope:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_member_function_create(
	struct RlcScopedMemberFunction * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberFunction const * parsed,
	struct RlcScopedScopeItemGroup * scope);

/** Destroys a scoped member function.
@memberof RlcScopedMemberFunction
@param[in,out] this:
	The scoped member function to destroy.
	@dassert @nonnull */
void rlc_scoped_member_function_destroy(
	struct RlcScopedMemberFunction * this);


#ifdef __cplusplus
}
#endif


#endif