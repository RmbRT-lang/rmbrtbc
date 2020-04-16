/** @file typedef.h
	Contains the scoped typedef type. */
#ifndef __rlc_scoper_typedef_h_defined
#define __rlc_scoper_typedef_h_defined

#include "scopeentry.h"
#include "member.h"
#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedTypedef;
struct RlcParsedMemberTypedef;

/** Scoped typedef type. */
struct RlcScopedTypedef
{
	/** The typedef's target type. */
	struct RlcScopedTypeName type;
};

/** Scoped global typedef.
@extends RlcScopedScopeEntry
@extends RlcScopedTypedef */
struct RlcScopedGlobalTypedef
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
	RLC_DERIVE(struct,RlcScopedTypedef);
};

/** Scoped member typedef.
@extends RlcScopedMember
@extends RlcScopedTypedef */
struct RlcScopedMemberTypedef
{
	RLC_DERIVE(struct,RlcScopedMember);
	RLC_DERIVE(struct,RlcScopedTypedef);
};

/** Creates a scoped global typedef from a parsed global typedef.
@memberof RlcScopedTypedef
@param[out] this:
	The scoped global typedef to create.
	@dassert @nonnull
@param[in] file:
	The typedef's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed global typedef.
	@dassert @nonnull
@param[in] group:
	The global typedef's scope item group.
	@dassert @nonnull */
void rlc_scoped_global_typedef_create(
	struct RlcScopedGlobalTypedef * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTypedef const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped typedef.
@memberof RlcScopedTypedef
@param[in,out] this:
	The scoped global typedef to destroy.
	@dassert @nonnull */
void rlc_scoped_global_typedef_destroy(
	struct RlcScopedGlobalTypedef * this);

/** Creates a scoped member typedef from a parsed member typedef.
@memberof RlcScopedTypedef
@param[out] this:
	The scoped member typedef to create.
	@dassert @nonnull
@param[in] file:
	The typedef's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member typedef.
	@dassert @nonnull
@param[in,out] scope:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_member_typedef_create(
	struct RlcScopedMemberTypedef * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberTypedef const * parsed,
	struct RlcScopedScopeItemGroup * scope);

/** Destroys a scoped member typedef.
@memberof RlcScopedTypedef
@param[in,out] this:
	The scoped member typedef to destroy.
	@dassert @nonnull */
void rlc_scoped_member_typedef_destroy(
	struct RlcScopedMemberTypedef * this);

#ifdef __cplusplus
}
#endif

#endif