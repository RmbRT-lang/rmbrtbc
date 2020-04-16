/** @file trystatement.h
	Contains the scoped try statement. */
#ifndef __rlc_scoper_try_statement_h_defined
#define __rlc_scoper_try_statement_h_defined

#include "statement.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcSrcFile;
struct RlcParsedTryStatement;
struct RlcParsedCatchStatement;

#define kRlcScopedTryStatement kRlcParsedTryStatement
#define kRlcScopedCatchStatement kRlcParsedCatchStatement

/** Scoped catch statement type. */
struct RlcScopedCatchStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);

	/** Whether it is a void catch. */
	int isVoid;
	/** The caught exception variable. */
	struct RlcScopedGlobalVariable * exception;
	/** The catch clause's body. */
	struct RlcScopedStatement * body;
};

/** Scoped try statement type.
@extends RlcScopedStatement */
struct RlcScopedTryStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);

	/** The try's body. */
	struct RlcScopedStatement * body;
	/** The catch clauses. */
	struct RlcScopedCatchStatement * catches;
	/** The catch clause count. */
	RlcSrcSize catchCount;
	/** The finally clause. */
	struct RlcScopedStatement * finally;
};

/** Creates a scoped try statement from a parsed try statement.
@memberof RlcScopedTryStatement
@param[out] this:
	The scoped try statement to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed try statement.
	@dassert @nonnull */
void rlc_scoped_try_statement_create(
	struct RlcScopedTryStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTryStatement const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped try statement.
@memberof RlcScopedTryStatement
@param[in,out] this:
	The scoped try statement to destroy.
	@dassert @nonnull */
void rlc_scoped_try_statement_destroy(
	struct RlcScopedTryStatement * this);

#ifdef __cplusplus
}
#endif

#endif