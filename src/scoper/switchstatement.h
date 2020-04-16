/** @file switchstatement.h
	Contains the scoped switch statement. */
#ifndef __rlc_scoper_switchstatement_h_defined
#define __rlc_scoper_switchstatement_h_defined
#pragma once

#include "statement.h"
#include "controllabel.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcSrcFile;
struct RlcParsedSwitchStatement;
struct RlcScopedCaseStatement;
struct RlcScopedScope;

#define kRlcScopedSwitchStatement kRlcParsedSwitchStatement

/** Scoped switch statement type.
@extends RlcScopedStatement */
struct RlcScopedSwitchStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);

	/** The switch statement's control label. */
	struct RlcScopedControlLabel label;
	/** The value to be switched. */
	struct RlcScopedExpression * switchValue;
	/** The switch statement's cases. */
	struct RlcScopedCaseStatement * cases;
	/** The switch statement's case count. */
	RlcSrcSize caseCount;
};

/** Creates a scoped switch statement from a parsed switch statement.
@memberof RlcScopedSwitchStatement
@param[out] this:
	The scoped switch statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed switch statement.
	@dassert @nonnull
@param[in,out] parent:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_switch_statement_create(
	struct RlcScopedSwitchStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSwitchStatement const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped switch statement.
@memberof RlcScopedSwitchStatement
@param[in,out] this:
	The scoped switch statement to destroy.
	@dassert @nonnull */
void rlc_scoped_switch_statement_destroy(
	struct RlcScopedSwitchStatement * this);

#ifdef __cplusplus
}
#endif

#endif