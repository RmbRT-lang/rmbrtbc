#ifndef __rlc_scoper_typename_h_defined
#define __rlc_scoper_typename_h_defined
#pragma once

#include "../src/file.h"
#include "../parser/typename.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedSymbol;
struct RlcScopedFunctionSignature;
struct RlcScopedExpression;
struct RlcParsedTypeName;

/** A scoped type name.
	Builds upon a parsed type name and refines it. */
struct RlcScopedTypeName
{
	/** Which kind of value the type name holds. */
	enum RlcParsedTypeNameValue value;
	/** The parsed type name. Contains the type's modifiers. */
	struct RlcParsedTypeName const * fParsed;
	/** Contains the scoped version of the type name's base type.
		The active field depends on the parsed type name's type. */
	union {
		/** The name of the type. */
		struct RlcScopedSymbol * fName;
		/** The function signature type. */
		struct RlcScopedFunctionSignature * fFunction;
		/** The expression (for type-of). */
		struct RlcScopedExpression * fExpression;
	};
};

/** Creates a scoped type name from a parsed type name.
@memberof RlcScopedTypeName
@param[out] this:
	The scoped type name to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed type name.
	@dassert @nonnull */
void rlc_scoped_type_name_create(
	struct RlcScopedTypeName * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTypeName const * parsed);

/** Destroys a scoped type name.
@memberof RlcScopedTypeName
@param[in,out] this:
	The type name to destroy.
	@dassert @nonnull */
void rlc_scoped_type_name_destroy(
	struct RlcScopedTypeName * this);

#ifdef __cplusplus
}
#endif

#endif