/** @file destructor.h
	Contains the scoped destructor type. */
#ifndef __rlc_scoper_destructor_h_defined
#define __rlc_scoper_destructor_h_defined
#pragma once

#include "member.h"
#include "blockstatement.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcSrcFile;
struct RlcParsedDestructor;

/** Scoped destructor type.
@extends RlcScopedMember */
struct RlcScopedDestructor
{
	RLC_DERIVE(struct,RlcScopedMember);

	/** Whether the destructor is inlined. */
	int isInline;
	/** Whether the destructor is defined. */
	int isDefinition;
	/** The destructor's body statement. */
	struct RlcScopedBlockStatement body;
};


/** Creates a scoped destructor from a parsed destructor.
@memberof RlcScopedDestructor
@param[out] this:
	The scoped destructor to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed destructor.
	@dassert @nonnull
@param[in,out] group:
	The destructor's scope item group. */
void rlc_scoped_destructor_create(
	struct RlcScopedDestructor * this,
	struct RlcSrcFile const * file,
	struct RlcParsedDestructor const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped destructor.
@memberof RlcScopedDestructor
@param[in,out] this:
	The scoped destructor to destroy.
	@dassert @nonnull */
void rlc_scoped_destructor_destroy(
	struct RlcScopedDestructor * this);

#ifdef __cplusplus
}
#endif


#endif