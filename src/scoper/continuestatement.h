/** @file continuestatement.h
	Contains the scoped continue statement. */
#ifndef __rlc_scoper_continuestatement_h_defined
#define __rlc_scoper_continuestatement_h_defined
#pragma once

#include "statement.h"
#include "controllabel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcParsedContinueStatement;

/** Scoped continue statement.
@extends RlcScopedStatement */
struct RlcScopedContinueStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);
	/** The continue statement's control label. */
	struct RlcScopedControlLabel label;
};

/** Creates a scoped continue statement from a parsed continue statement.
@memberof RlcScopedContinueStatement
@param[out] this:
	The scoped continue statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed continue statement.
	@dassert @nonnull */
void rlc_scoped_continue_statement_create(
	struct RlcScopedContinueStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedContinueStatement const * parsed);

/** Destroys a scoped continue statement.
@memberof RlcScopedContinueStatement
@param[in,out] this:
	The scoped continue statement to destroy.
	@dassert @nonnull */
void rlc_scoped_continue_statement_destroy(
	struct RlcScopedContinueStatement * this);

#ifdef __cplusplus
}
#endif

#endif