/** @file member.h
	Contains the scoped member type. */
#ifndef __rlc_scoper_member_h_defined
#define __rlc_scoper_member_h_defined
#pragma once

#include "scopeitem.h"
#include "../parser/member.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedMember;

#define RlcScopedMemberType RlcParsedMemberType

/** Type member declaration as used by the scoper. */
struct RlcScopedMember
{
	RLC_ABSTRACT(RlcScopedMember);
	RLC_DERIVE(struct,RlcScopedScopeItem);

	/** The member's parsed representation. */
	struct RlcParsedMember const * parsed;
};

/** Creates a scoped member from a parsed member.
@memberof RlcScopedMember
@param[out] this:
	The scoped member to create.
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member.
	@dassert @nonnull
@param[in] parent:
	The scope the member belongs to.
	@dassert @nonnull
@param[in] type:
	The deriving type. */
void rlc_scoped_member_create(
	struct RlcScopedMember * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMember const * parsed,
	struct RlcScopedScope * parent,
	enum RlcScopedMemberType type);

/** Creates a new scoped member from a parsed member.
@memberof RlcScopedMember
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member.
	@dassert @nonnull
@param[in] parent:
	The scope entry the member belongs to.
	@dassert @nonnull
@return
	The scoped member. */
struct RlcScopedMember * rlc_scoped_member_new(
	struct RlcSrcFile const * file,
	struct RlcParsedMember const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped member.
@memberof RlcScopedMember
@param[in,out] this:
	The member to destroy.
	@dassert @nonnull */
void rlc_scoped_member_destroy_base(
	struct RlcScopedMember * this);

void rlc_scoped_member_destroy_virtual(
	struct RlcScopedMember * this);

#ifdef __cplusplus
}
#endif


#endif