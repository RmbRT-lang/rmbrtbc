/** @file breakstatement.h
	Contains the scoped break statement. */
#ifndef __rlc_scoper_breakstatement_h_defined
#define __rlc_scoper_breakstatement_h_defined
#pragma once

#include "statement.h"
#include "controllabel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcParsedBreakStatement;

/** Scoped break statement.
@extends RlcScopedStatement */
struct RlcScopedBreakStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);
	/** The break statement's control label. */
	struct RlcScopedControlLabel label;
};

/** Creates a scoped break statement from a parsed break statement.
@memberof RlcScopedBreakStatement
@param[out] this:
	The scoped break statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed break statement.
	@dassert @nonnull */
void rlc_scoped_break_statement_create(
	struct RlcScopedBreakStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedBreakStatement const * parsed);

/** Destroys a scoped break statement.
@memberof RlcScopedBreakStatement
@param[in,out] this:
	The scoped break statement to destroy.
	@dassert @nonnull */
void rlc_scoped_break_statement_destroy(
	struct RlcScopedBreakStatement * this);

#ifdef __cplusplus
}
#endif

#endif