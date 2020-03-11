/** @file rawtype.h
	Contains the scoped rawtype type. */
#ifndef __rlc_scoper_rawtype_h_defined
#define __rlc_scoper_rawtype_h_defined
#pragma once

#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedRawtype;
struct RlcScopedExpression;

/** Rawtype class as used by the scoper.
@extends RlcScopedScopeEntry */
struct RlcScopedRawtype
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
	/** The rawtype's size expression. */
	struct RlcScopedExpression * fSize;
};

/** Creates a scoped rawtype from a parsed rawtype.
@memberof RlcScopedRawtype
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed rawtype.
	@dassert @nonnull
@param[in] parent:
	The parent scope.
	@dassert @nonnull
@return
	The scoped rawtype. */
struct RlcScopedRawtype * rlc_scoped_rawtype_new(
	struct RlcSrcFile const * file,
	struct RlcParsedRawtype const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Deletes a scoped rawtype.
@memberof RlcScopedRawtype
@param[in,out] this:
	The rawtype to delete.
	@dassert @nonnull */
void rlc_scoped_rawtype_delete(
	struct RlcScopedRawtype * this);

#ifdef __cplusplus
}
#endif


#endif