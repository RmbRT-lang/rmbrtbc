/** @file blockstatement.h
	Contains the scoped block statement type. */
#ifndef __rlc_scoper_blockstatement_h_defined
#define __rlc_scoper_blockstatement_h_defined
#pragma once

#include "statement.h"
#include "../src/string.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedBlockStatement;
struct RlcSrcFile;

/** Scoped block statement type.
@extends RlcScopedStatement */
struct RlcScopedBlockStatement
{
	RLC_DERIVE(struct,RlcScopedStatement);
	/** The sub-statements. */
	struct RlcScopedStatement ** statements;
	/** The sub-statement count. */
	RlcSrcSize statementCount;
};

/** Creates a scoped block statement from a parsed block statement.
@memberof RlcScopedBlockStatement
@param[out] this:
	The block statement to create.
	@dassert @nonnull
@param[in] file:
	The block statement's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed block statement.
	@dassert @nonnull */
void rlc_scoped_block_statement_create(
	struct RlcScopedBlockStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedBlockStatement const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped block statement.
@memberof RlcScopedBlockStatement
@param[in,out] this:
	The block statement to destroy.
	@dassert @nonnull */
void rlc_scoped_block_statement_destroy(
	struct RlcScopedBlockStatement * this);

#ifdef __cplusplus
}
#endif

#endif