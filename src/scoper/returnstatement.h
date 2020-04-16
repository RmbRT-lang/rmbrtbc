/** @file returnstatement.h
	Contains the scoped return statement type. */
#ifndef __rlc_scoper_returnstatement_h_defined
#define __rlc_scoper_returnstatement_h_defined
#pragma once

#include "statement.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcSrcFile;
struct RlcScopedExpression;
struct RlcParsedReturnStatement;

#define kRlcScopedReturnStatement kRlcParsedReturnStatement

/** Scoped return statement type.
@extends RlcScopedStatement */
struct RlcScopedReturnStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);
	/** The returned value, if any. */
	struct RlcScopedExpression * value;
};

/** Creates a scoped return statement from a parsed return statement.
@memberof RlcScopedReturnStatement
@param[out] this:
	The scoped return statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed return statement.
	@dassert @nonnull */
void rlc_scoped_return_statement_create(
	struct RlcScopedReturnStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedReturnStatement const * parsed);

/** Destroys a scoped return statement.
@memberof RlcScopedReturnStatement
@param[in,out] this:
	The scoped return statement to destroy.
	@dassert @nonnull */
void rlc_scoped_return_statement_destroy(
	struct RlcScopedReturnStatement * this);

#ifdef __cplusplus
}
#endif

#endif