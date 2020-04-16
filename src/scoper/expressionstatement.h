/** @file expressionstatement.h
	Contains the scoped expression statement type. */
#ifndef __rlc_scoper_expressionstatement_h_defined
#define __rlc_scoper_expressionstatement_h_defined
#pragma once

#include "statement.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedExpressionStatement;

/** Scoped expression statement type.
@extends RlcScopedStatement */
struct RlcScopedExpressionStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);
	/** The statement's expression. */
	struct RlcScopedExpression * expression;
};

/** Creates a scoped expression statement from a parsed expression statement.
@memberof RlcScopedExpressionStatement
@param[out] this:
	The scoped expression statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed expression statement.
	@dassert */
void rlc_scoped_expression_statement_create(
	struct RlcScopedExpressionStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedExpressionStatement const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped expression statement.
@memberof RlcScopedExpressionStatement
@param[in,out] this:
	The scoped expression statement to destroy.
	@dassert @nonnull */
void rlc_scoped_expression_statement_destroy(
	struct RlcScopedExpressionStatement * this);

#ifdef __cplusplus
}
#endif

#endif