/** @file typename.h
	Contains the declarations of the type name and related structures. */
#ifndef __rlc_parser_typename_hpp_defined
#define __rlc_parser_typename_hpp_defined

#include "symbol.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/** Type indirection. */
typedef enum RlcTypeIndirection
{
	/** Plain qualifier - instance. */
	kRlcTypeIndirectionPlain,
	/** Not null qualifier - non-null pointer. */
	kRlcTypeIndirectionNotNull,
	/** Pointer qualifier - nullable pointer. */
	kRlcTypeIndirectionPointer
} RlcTypeIndirection;

/** CV qualifier flag enum. */
typedef enum RlcCVQualifier
{
	/** Modifiable, non-volatile. */
	kRlcCVQualifierNone
	/** Const qualifier. */
	kRlcCVQualifierConst,
	/** Volatile qualifier. */
	kRlcCVQualifierVolatile
} RlcCVQualifier;

/** A type modifier that consists of an indirection and a cv qualifier. */
struct RlcTypeModifier
{
	/** The type indirection. */
	RlcTypeIndirection fTypeIndirection;
	/** The cv qualifier. */
	RlcCVQualifier fCVQualifier;
};

/** A (possibly) namespace-qualified identifier, or void, and type qualifiers. */
struct RlcTypeName
{
	/** The name of the type. */
	struct RlcSymbol fName;
	/** Whether the type name is void.
		If true, the name field is unused. */
	int fIsVoid;

	/** The type modifiers list. */
	struct RlcTypeModifier * fTypeModifiers;
	/** The type modifier count. */
	size_t fTypeModifierCount;
};

#ifdef __cplusplus
}
#endif

#endif