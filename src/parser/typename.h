/** @file typename.h
	Contains the declarations of the type name and related structures. */
#ifndef __rlc_parser_typename_hpp_defined
#define __rlc_parser_typename_hpp_defined

#include "symbol.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Type indirection.
	@see
		`rlc_type_indirection_parse()`. */
enum RlcTypeIndirection
{
	/** Plain qualifier - instance. */
	kRlcTypeIndirectionPlain,
	/** Not null qualifier - non-null pointer. */
	kRlcTypeIndirectionNotNull,
	/** Pointer qualifier - nullable pointer. */
	kRlcTypeIndirectionPointer,

	RLC_ENUM_END(RlcTypeIndirection)
};

/** Parses a type indirection.
@memberof RlcParserData
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[out] out:
	The indirection to parse.
	@dassert @nonnull */
void rlc_type_indirection_parse(
	struct RlcParserData * parser,
	enum RlcTypeIndirection * out);

/** CV qualifier flag enum. */
enum RlcTypeQualifier
{
	/** No qualifier. */
	kRlcTypeQualifierNone,
	/** Const qualifier. */
	kRlcTypeQualifierConst,
	/** Volatile qualifier. */
	kRlcTypeQualifierVolatile,

	RLC_ENUM_END(RlcTypeQualifier)
};

/** Parses a type qualifier.
@memberof RlcParserData
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[out] out:
	The type qualifier to parse.
	@dassert @nonnull */
void rlc_type_qualifier_parse(
	struct RlcParserData * parser,
	enum RlcTypeQualifier * out);

/** A type modifier that consists of an indirection and a cv qualifier. */
struct RlcTypeModifier
{
	/** The type indirection. */
	enum RlcTypeIndirection fTypeIndirection;
	/** The cv qualifier. */
	enum RlcTypeQualifier fTypeQualifier;
};

/** Parses a type modifier.
@memberof RlcParserData
@relates RlcTypeModifier */
void rlc_type_modifier_parse(
	struct RlcParserData * parser,
	struct RlcTypeModifier * out);

/** A (possibly) scope-qualified identifier, or void, and type qualifiers. */
struct RlcParsedTypeName
{
	/** The name of the type. */
	struct RlcParsedSymbol fName;
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
	The type name to destroy.
	@dassert @nonnull */
void rlc_parsed_type_name_destroy(
	struct RlcParsedTypeName * this);
/** Adds a type modifier to the type name.
@param[in,out] this:
	The type name to add a modifier to.
	@dassert @nonnull
@param[in] modifier:
	The modifier to add to the type name. Must not be a modifier owned by the type name, because this would result in a segmentation fault in case memory had to be moved to expand the type modifier array.
	@dassert @nonnull */
void rlc_parsed_type_name_add_modifier(
	struct RlcParsedTypeName * this,
	struct RlcTypeModifier const * modifier);

/** Creates a parsed type name.
@param[in,out] this:
	The parsed type name to create.
	@dassert @nonnull */
void rlc_parsed_type_name_create(
	struct RlcParsedTypeName * this);

/** Parses a type name.
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[out] out:
	The type name to parse into.
	@dassert @nonnull
@return
	Nonzero on success, 0 on error. */
int rlc_parsed_type_name_parse(
	struct RlcParserData * parser,
	struct RlcParsedTypeName * out);

#ifdef __cplusplus
}
#endif

#endif