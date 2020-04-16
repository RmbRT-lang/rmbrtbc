/** @file class.h
	Contains the scoped class type. */
#ifndef __rlc_scoper_class_h_defined
#define __rlc_scoper_class_h_defined
#pragma once

#include "scopeentry.h"
#include "member.h"
#include "symbol.h"
#include "../parser/member.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedClass;
struct RlcParsedMemberClass;
struct RlcParsedInheritance;

struct RlcScopedDestructor;

/** Scoped type inheritance list entry. */
struct RlcScopedInheritance
{
	/** Whether the inheritance is virtual. */
	int isVirtual;
	/** The inheritance's visibility. */
	enum RlcVisibility visibility;
	/** The base class to inherit. */
	struct RlcScopedSymbol base;
};

/** Scoped class type. */
struct RlcScopedClass
{
	/** The class' inheritance list. */
	struct RlcScopedInheritance * inheritances;
	/** The class' inheritance list size. */
	RlcSrcSize inheritanceCount;

	/** The class' fields. */
	struct RlcScopedMemberVariable ** fields;
	/** The class' field count. */
	RlcSrcSize fieldCount;
};

/** Global scope classes as used by the scoper.
@extends RlcScopedScopeEntry
@extends RlcScopedClass */
struct RlcScopedGlobalClass
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
	RLC_DERIVE(struct,RlcScopedClass);
};

/** Member class type.
@implements RlcScopedMember
@extends RlcScopedClass */
struct RlcScopedMemberClass
{
	RLC_DERIVE(struct,RlcScopedMember);
	RLC_DERIVE(struct,RlcScopedClass);
};

/** Creates a scoped global class from a parsed global class.
@memberof RlcScopedClass
@param[out] this:
	The scoped class to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed class.
	@dassert @nonnull
@param[in] parent:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_global_class_create(
	struct RlcScopedGlobalClass * this,
	struct RlcSrcFile const * file,
	struct RlcParsedClass const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped class.
@memberof RlcScopedClass
@param[in,out] this:
	The class to destroy.
	@dassert @nonnull */
void rlc_scoped_global_class_destroy(
	struct RlcScopedGlobalClass * this);


/** Creates a scoped member class from a parsed member class.
@memberof RlcScopedMemberClass
@param[out] this:
	The scoped member class to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member class.
	@dassert @nonnull
@param[in] parent:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_member_class_create(
	struct RlcScopedMemberClass * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberClass const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped class.
@memberof RlcScopedMemberClass
@param[in,out] this:
	The member class to destroy.
	@dassert @nonnull */
void rlc_scoped_member_class_destroy(
	struct RlcScopedMemberClass * this);

#ifdef __cplusplus
}
#endif


#endif