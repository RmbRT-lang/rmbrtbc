/** @file variablestatement.h
	Contains the scoped variable statement type. */
#ifndef __rlc_scoper_variablestatement_h_defined
#define __rlc_scoper_variablestatement_h_defined
#pragma once

#include "statement.h"


#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcParsedVariableStatement;
struct RlcScopedScope;

#define kRlcScopedVariableStatement kRlcParsedVariableStatement

/** Scoped variable statement.
@extends RlcScopedStatement */
struct RlcScopedVariableStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);
	/** The statement's variable. */
	struct RlcScopedGlobalVariable * variable;
};

/** Creates a scoped variable statement from a parsed variable statement.
@memberof RlcScopedVariableStatement
@param[out] this:
	The scoped variable statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed variable statement.
	@dassert @nonnull
@param[in,out] parent:
	The scope to insert the variable into.
	@dassert @nonnull */
void rlc_scoped_variable_statement_create(
	struct RlcScopedVariableStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariableStatement const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped variable statement.
@memberof RlcScopedVariableStatement
@param[in,out] this:
	The scoped variable statement to destroy.
	@dassert @nonnull */
void rlc_scoped_variable_statement_destroy(
	struct RlcScopedVariableStatement * this);

#ifdef __cplusplus
}
#endif

#endif