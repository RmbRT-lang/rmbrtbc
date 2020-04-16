/** @file union.h
	Contains the scoped union types. */
#ifndef __rlc_scoper_union_h_defined
#define __rlc_scoper_union_h_defined
#pragma once

#include "scopeentry.h"
#include "member.h"

#include "../src/string.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcSrcFile;
struct RlcParsedUnion;
struct RlcParsedMemberUnion;
struct RlcScopedMemberVariable;

/** Scoped union type. */
struct RlcScopedUnion
{
	/** The enum's fields. */
	struct RlcScopedMemberVariable ** fields;
	/** The number of the enum's fields. */
	RlcSrcSize fieldCount;
};

/** Scoped global union type.
@extends RlcScopedUnion
@extends RlcScopedScopeEntry */
struct RlcScopedGlobalUnion
{
	RLC_DERIVE(struct,RlcScopedUnion);
	RLC_DERIVE(struct,RlcScopedScopeEntry);
};

/** Scoped member union type.
@extends RlcScopedUnion
@extends RlcScopedMember */
struct RlcScopedMemberUnion
{
	RLC_DERIVE(struct,RlcScopedUnion);
	RLC_DERIVE(struct,RlcScopedMember);
};


/** Creates a scoped global union from a parsed global union.
@memberof RlcScopedGlobalUnion
@param[out] this:
	The scoped  global union to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed global union.
	@dassert @nonnull
@param[in,out] group:
	The scope item group.
	@dassert @nonnull */
void rlc_scoped_global_union_create(
	struct RlcScopedGlobalUnion * this,
	struct RlcSrcFile const * file,
	struct RlcParsedUnion const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped global union.
@memberof RlcScopedGlobalUnion
@param[in,out] this:
	The scoped global union to destroy.
	@dassert @nonnull */
void rlc_scoped_global_union_destroy(
	struct RlcScopedGlobalUnion * this);

/** Creates a scoped member union from a parsed member union.
@memberof RlcScopedMemberUnion
@param[out] this:
	The scoped  member union to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member union.
	@dassert @nonnull
@param[in,out] group:
	The scope item group.
	@dassert @nonnull */
void rlc_scoped_member_union_create(
	struct RlcScopedMemberUnion * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberUnion const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped member union.
@memberof RlcScopedMemberUnion
@param[in,out] this:
	The scoped member union to destroy.
	@dassert @nonnull */
void rlc_scoped_member_union_destroy(
	struct RlcScopedMemberUnion * this);

#ifdef __cplusplus
}
#endif


#endif