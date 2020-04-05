/** @file class.h
	Contains the scoped class type. */
#ifndef __rlc_scoper_class_h_defined
#define __rlc_scoper_class_h_defined
#pragma once

#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedClass;

/** Class class as used by the scoper.
@extends RlcScopedScopeEntry */
struct RlcScopedClass
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
};

/** Creates a scoped class from a parsed class.
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
	@dassert @nonnull
@return
	The scoped class. */
void rlc_scoped_class_create(
	struct RlcScopedClass * this,
	struct RlcSrcFile const * file,
	struct RlcParsedClass const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped class.
@memberof RlcScopedClass
@param[in,out] this:
	The class to destroy.
	@dassert @nonnull */
void rlc_scoped_class_destroy(
	struct RlcScopedClass * this);


#ifdef __cplusplus
}
#endif


#endif