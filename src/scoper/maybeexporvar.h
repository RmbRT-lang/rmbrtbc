/** @file maybeexporvar.h
	Contains the expression or variable union type. */
#ifndef __rlc_scoper_maybeexporvar_h_defined
#define __rlc_scoper_maybeexporvar_h_defined
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcParsedExpression;
struct RlcParsedVariable;
struct RlcScopedScope;

/** Union containing either a scoped expression or variable or nothing. */
struct RlcScopedMaybeExpOrVar
{
	/** Whether there is a value. */
	char exists;
	/** Whether the value is an expression. */
	char isExpression;
	union {
		struct RlcScopedExpression * expression;
		struct RlcScopedVariable * variable;
	};
};

/** Creates an ExpOrVar with an expression.
@memberof RlcScopedMaybeExpOrVar
@param[out] this:
	The ExpOrVar to create.
	@dassert @nonnull
@param[in] file:
	The expression's source file.
	@dassert @nonnull
@param[in] expression:
	The expression.
	@dassert @nonnull */
void rlc_scoped_maybe_exp_or_var_create_expression(
	struct RlcScopedMaybeExpOrVar * this,
	struct RlcSrcFile const * file,
	struct RlcParsedExpression const * expression);

void rlc_scoped_maybe_exp_or_var_create_empty(
	struct RlcScopedMaybeExpOrVar * this);

/** Creates an ExpOrVar with a variable.
@memberof RlcScopedMaybeExpOrVar
@param[out] this:
	The ExpOrVar to create.
	@dassert @nonnull
@param[in] file:
	The variable's source file.
	@dassert @nonnull
@param[in] variable:
	The variable.
	@dassert @nonnull */
void rlc_scoped_maybe_exp_or_var_create_variable(
	struct RlcScopedMaybeExpOrVar * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariable const * variable,
	struct RlcScopedScope * scope);

/** Destroys an ExpOrVar.
@memberof RlcScopedMaybeExpOrVar
@param[in,out] this:
	The ExpOrVar to destroy.
	@dassert @nonnull */
void rlc_scoped_maybe_exp_or_var_destroy(
	struct RlcScopedMaybeExpOrVar * this);

#ifdef __cplusplus
}
#endif

#endif