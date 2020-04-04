/** @file statement.h
	Contains the scoped statement type. */
#ifndef __rlc_scoper_statement_h_defined
#define __rlc_scoper_statement_h_defined

#include "../macros.h"
#include "../parser/statement.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RlcScopedStatementType RlcParsedStatementType

/** Scoped statement type. */
struct RlcScopedStatement
{
	RLC_ABSTRACT(RlcScopedStatement);
	/** The statement's scope (if applicable). */
	struct RlcScopedScope * scope;
	/** The parsed statement. */
	struct RlcParsedStatement const * parsed;
};

/** Creates a scoped statement from a parsed statement.
@memberof RlcScopedStatement
@param[in] file:
	The statement's source file.
	@dassert @nonnull
@param[in] file:
	The parsed statement.
	@dassert @nonnull
@return
	The scoped statement. */
struct RlcScopedStatement * rlc_scoped_statement_new(
	struct RlcSrcFile const * file,
	struct RlcParsedStatement const * parsed);

/** Destroys and deletes a scoped statement.
@memberof RlcScopedStatement
@param[in,out] this:
	The scoped statement to destroy and delete.
	@dassert @nonnull */
void rlc_scoped_statement_delete(
	struct RlcScopedStatement * this);

/** Creates a scoped statement.
@memberof RlcScopedStatement
@param[out] this:
	The scoped statement to create.
	@dassert @nonnull
@param[in] parsed:
	The parsed statement.
	@dassert @nonnull
@param[in] type:
	The scope statement's deriving type.
	@dassert @nonnull
@param[in] make_scope:
	Whether to create a scope for the statement. */
void rlc_scoped_statement_create(
	struct RlcScopedStatement * this,
	struct RlcParsedStatement const * parsed,
	enum RlcScopedStatementType type,
	int make_scope);

/** Destroys a scoped statement.
@memberof RlcScopedStatement
@param[in,out] this:
	The scoped statement to destroy.
	@dassert @nonnull */
void rlc_scoped_statement_destroy_base(
	struct RlcScopedStatement * this);

#ifdef __cplusplus
}
#endif

#endif