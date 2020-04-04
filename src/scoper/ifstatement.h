/** @file ifstatement.h
	Contains the scoped if statement. */
#ifndef __rlc_scoper_ifstatement_h_defined
#define __rlc_scoper_ifstatement_h_defined
#pragma once

#include "statement.h"
#include "maybeexporvar.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedIfStatement;
struct RlcScopedIdentifier;

/** Scoped if statement.
@extends RlcScopedStatement */
struct RlcScopedIfStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);

	/** The if statement's condition. */
	struct RlcScopedMaybeExpOrVar condition;

	/** The if statement's control label, if any. */
	struct RlcScopedIdentifier * label;

	/** The if statement's if body. */
	struct RlcScopedStatement * ifBody;
	/** The if statement's else body, if any. */
	struct RlcScopedStatement * elseBody;
};

/** Creates a scoped if statement from a parsed if statement.
@memberof RlcScopedIfStatement
@param[out] this:
	The scoped if statement to create.
	@dassert @nonnull
@param[in] file:
	The statement's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed if statement.
	@dassert @nonnull */
void rlc_scoped_if_statement_create(
	struct RlcScopedIfStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedIfStatement const * parsed);

/** Destroys a scoped if statement.
@memberof RlcScopedIfStatement
@param[in,out] this:
	The scoped if statement to destroy.
	@dassert @nonnull */
void rlc_scoped_if_statement_destroy(
	struct RlcScopedIfStatement * this);


#ifdef __cplusplus
}
#endif

#endif