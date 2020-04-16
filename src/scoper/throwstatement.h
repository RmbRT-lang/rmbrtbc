/** @file throwstatement.h
	Contains the scoped throw statement type. */
#ifndef __rlc_scoper_throwstatement_h_defined
#define __rlc_scoper_throwstatement_h_defined
#pragma once

#include "statement.h"
#include "../parser/throwstatement.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcParsedThrowStatement;
struct RlcScopedExpression;

#define kRlcScopedThrowStatement kRlcParsedThrowStatement

/** Scoped throw statement.
@extends RlcScopedStatement */
struct RlcScopedThrowStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);

	/** The throw statement's type. */
	enum RlcThrowType type;
	/** The value to be thrown (if applicable to the type). */
	struct RlcScopedExpression * value;
};

/** Creates a scoped throw statement from a parsed throw statement.
@memberof RlcScopedThrowStatement
@param[out] this:
	The throw statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed throw statement.
	@dassert @nonnull */
void rlc_scoped_throw_statement_create(
	struct RlcScopedThrowStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedThrowStatement const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped throw statement.
@memberof RlcScopedThrowStatement
@param[in,out] this:
	The scoped throw statement to destroy.
	@dassert @nonnull */
void rlc_scoped_throw_statement_destroy(
	struct RlcScopedThrowStatement * this);

#ifdef __cplusplus
}
#endif


#endif