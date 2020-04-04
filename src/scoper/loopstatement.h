/** @file loopstatement.h
	Contains the scoped loop statement. */
#ifndef __rlc_scoper_loopstatement_h_defined
#define __rlc_scoper_loopstatement_h_defined
#pragma once

#include "statement.h"
#include "maybeexporvar.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedLoopStatement;
struct RlcScopedIdentifier;

/** Scoped loop statement type.
@extends RlcScopedStatement */
struct RlcScopedLoopStatement
{
	RLC_DERIVE(struct, RlcScopedStatement);

	/** The loop's control label. */
	struct RlcScopedIdentifier * label;
	/** Whether the condition is checked before or after the loop body. */
	int isPostCondition;
	/** The loop statement's initial statement, if any. */
	struct RlcScopedMaybeExpOrVar initial;
	/** The loop statement's condition, if any. */
	struct RlcScopedMaybeExpOrVar condition;
	/** The loop statement's post-loop statement, if any. */
	struct RlcScopedExpression * postLoop;
	/** The loop's body statement, if any. */
	struct RlcScopedStatement * body;
};

/** Creates a scoped loop statement from a parsed loop statement.
@memberof RlcScopedLoopStatement
@param[out] this:
	The scoped loop statement to create.
	@dassert @nonnull
@param[in] file:
	The loop statement's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed loop statement.
	@dassert @nonnull */
void rlc_scoped_loop_statement_create(
	struct RlcScopedLoopStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedLoopStatement * parsed);

/** Destroys a scoped loop statement.
@memberof RlcScopedLoopStatement
@param[in,out] this:
	The scoped loop statement to destroy.
	@dassert @nonnull */
void rlc_scoped_loop_statement_destroy(
	struct RlcScopedLoopStatement * this);

#ifdef __cplusplus
}
#endif

#endif