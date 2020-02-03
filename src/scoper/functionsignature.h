/** @file functionsignature.h
	Contains the function signature type as used by the scoper. */
#ifndef __rlc_scoper_functionsignature_h_defined
#define __rlc_scoper_functionsignature_h_defined
#pragma once

#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;
struct RlcParsedFunctionSignature;

/** Function signature type used by the scoper.
	Redundant information is stored in its parsed counterpart. */
struct RlcScopedFunctionSignature
{
	/** The function's arguments. */
	struct RlcScopedTypeName * fArguments;
	/** The function's return type. */
	struct RlcScopedTypeName fResult;
};

/** Creates a scoped function signature from a parsed signature.
@memberof RlcScopedFunctionSignature
@param[out] this:
	The scoped function signature to create.
	@dassert @nonnull
@param[in] file:
	The parsed signature's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed signature.
	@dassert @nonnull */
void rlc_scoped_function_signature_create(
	struct RlcScopedFunctionSignature * this,
	struct RlcSrcFile const * file,
	struct RlcParsedFunctionSignature const * parsed);

/** Destroys a scoped function signature.
@memberof RlcScopedFunctionSignature
@param[in,out] this:
	The scoped function signature to destroy.
	@dassert @nonnull
@param[in] parsed:
	The parsed function signature.
	@dassert @nonnull */
void rlc_scoped_function_signature_destroy(
	struct RlcScopedFunctionSignature * this,
	struct RlcParsedFunctionSignature const * parsed);


#ifdef __cplusplus
}
#endif

#endif