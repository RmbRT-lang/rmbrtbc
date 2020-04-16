/** @file rawtype.h
	Contains the scoped rawtype type. */
#ifndef __rlc_scoper_rawtype_h_defined
#define __rlc_scoper_rawtype_h_defined
#pragma once

#include "scopeentry.h"
#include "member.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedRawtype;
struct RlcParsedMemberRawtype;
struct RlcScopedExpression;

/** Rawtype class used by the scoper. */
struct RlcScopedRawtype
{
	/** The rawtype's size expression. */
	struct RlcScopedExpression * size;
};

/** Global rawtype class as used by the scoper.
@extends RlcScopedScopeEntry
@extends RlcScopedRawtype */
struct RlcScopedGlobalRawtype
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
	RLC_DERIVE(struct,RlcScopedRawtype);
};

/** Rawtype class as used by the scoper.
@extends RlcScopedMember
@extends RlcScopedRawtype */
struct RlcScopedMemberRawtype
{
	RLC_DERIVE(struct,RlcScopedMember);
	RLC_DERIVE(struct,RlcScopedRawtype);
};

/** Creates a scoped rawtype from a parsed rawtype.
@memberof RlcScopedRawtype
@param[out] this:
	The scoped rawtype to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed rawtype.
	@dassert @nonnull
@param[in] parent:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_global_rawtype_create(
	struct RlcScopedGlobalRawtype * this,
	struct RlcSrcFile const * file,
	struct RlcParsedRawtype const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped rawtype.
@memberof RlcScopedRawtype
@param[in,out] this:
	The rawtype to destroy.
	@dassert @nonnull */
void rlc_scoped_global_rawtype_destroy(
	struct RlcScopedGlobalRawtype * this);

/** Creates a scoped member rawtype from a parsed member rawtype.
@memberof RlcScopedMemberRawtype
@param[out] this:
	The scoped member rawtype to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member rawtype.
	@dassert @nonnull
@param[in,out] parent:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_member_rawtype_create(
	struct RlcScopedMemberRawtype * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberRawtype const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped member rawtype.
@memberof RlcScopedMemberRawtype
@param[in,out] this:
	The scoped member rawtype to create.
	@dassert @nonnull */
void rlc_scoped_member_rawtype_destroy(
	struct RlcScopedMemberRawtype * this);

#ifdef __cplusplus
}
#endif


#endif