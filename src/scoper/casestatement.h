/** @file casestatement.h
	Contains the scoped case statement. */
#ifndef __rlc_scoper_casestatement_h_defined
#define __rlc_scoper_casestatement_h_defined
#pragma once

#include "statement.h"
#include "controllabel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedCaseStatement;

#define kRlcScopedCaseStatement kRlcParsedCaseStatement

/** Scoped case statement type.
@extends RlcScopedStatement */
struct RlcScopedCaseStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);

	/** The case's control label. */
	struct RlcScopedControlLabel controlLabel;
	/** The case's values.
		Only exists if it is not the default case. */
	struct RlcScopedExpression ** values;
	/** The case's value count. */
	RlcSrcSize valueCount;
	/** Whether it is the default case. */
	char isDefault;

	/** The case's body statement. */
	struct RlcScopedStatement * body;
};

/** Creates a scoped case statement from a parsed case statement.
@memberof RlcScopedCaseStatement
@param[out] this:
	The scoped case statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed case statement.
	@dassert @nonnull
@param[in,out] parent:
	The parent scope.
	@dassert @nonnull  */
void rlc_scoped_case_statement_create(
	struct RlcScopedCaseStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedCaseStatement const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped case statement.
@memberof RlcScopedCaseStatement
@param[in,out] this:
	The scoped case statement to destroy.
	@dassert @nonnull */
void rlc_scoped_case_statement_destroy(
	struct RlcScopedCaseStatement * this);


#ifdef __cplusplus
}
#endif

#endif