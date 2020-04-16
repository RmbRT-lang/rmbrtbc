/** @file constructor.h
	The scoped constructor types. */
#ifndef __rlc_scoper_constructor_h_defined
#define __rlc_scoper_constructor_h_defined
#pragma once

#include "member.h"
#include "blockstatement.h"
#include "identifier.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedConstructor;
struct RlcParsedInitialiser;
struct RlcScopedExpression;

/** Scoped member initialiser statement. */
struct RlcScopedInitialiser
{
	/** The member to be initialised. */
	struct RlcScopedIdentifier member;
	/** The member's constructor arguments. */
	struct RlcScopedExpression ** arguments;
	/** The member's constructor argument count. */
	RlcSrcSize argumentCount;
};

/** Scoped constructor type.
@extends RlcScopedMember */
struct RlcScopedConstructor
{
	RLC_DERIVE(struct,RlcScopedMember);

	/** The arguments. */
	struct RlcScopedGlobalVariable ** arguments;
	/** The argument count. */
	size_t argumentCount;

	/** Whether the constructor is a definition. */
	int isDefinition;

	/** The initialisers. */
	struct RlcScopedInitialiser * initialisers;
	/** The initialiser count. */
	RlcSrcSize initialiserCount;

	/** The body statement. */
	struct RlcScopedBlockStatement body;
	/** Whether the constructor is inline. */
	int isInline;
};

/** Creates a scoped constructor from a parsed constructor.
@memberof RlcScopedConstructor
@param[out] this:
	The scoped constructor to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed constructor.
	@dassert @nonnull
@param[in,out] group:
	The constructor's scope item group.
	@dassert @nonnull */
void rlc_scoped_constructor_create(
	struct RlcScopedConstructor * this,
	struct RlcSrcFile const * file,
	struct RlcParsedConstructor const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped constructor.
@memberof RlcScopedConstructor
@param[in,out] this:
	The scoped constructor to destroy.
	@dassert @nonnull */
void rlc_scoped_constructor_destroy(
	struct RlcScopedConstructor * this);


#ifdef __cplusplus
}
#endif

#endif