/** @file typename.h
	Contains the declarations of the type name and related structures. */
#ifndef __rlc_parser_typename_hpp_defined
#define __rlc_parser_typename_hpp_defined

#include "symbol.h"

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
	/** No qualifier. */
	kRlcCVQualifierNone,
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

/** A (possibly) scope-qualified identifier, or void, and type qualifiers. */
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
/** Destroys a type name.
	Releases memory allocated by the given type name, but not the type name itself.
@param[in] this:
	The type name to destroy. */
void rlc_type_name_destroy(
	struct RlcTypeName * this);
/** Adds a type modifier to the type name.
@param[in] this:
	The type name to add a modifier to.
@param[in] modifier:
	The modifier to add to the type name. Must not be a modifier owned by the type name, because this would result in a segmentation fault in case memory had to be moved to expand the type modifier array. */
void rlc_type_name_add_modifier(
	struct RlcTypeName * this,
	struct RlcTypeModifier const * modifier);

#ifdef __cplusplus
}
#endif

#endif