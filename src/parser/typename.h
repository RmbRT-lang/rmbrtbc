/** @file typename.h
	Contains the declarations of the type name and related structures. */
#ifndef __rlc_parser_typename_hpp_defined
#define __rlc_parser_typename_hpp_defined

#include "symbol.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Reference types. */
enum RlcReferenceType
{
	/** Not a reference. */
	kRlcReferenceTypeNone,
	/** A normal const reference. */
	kRlcReferenceTypeReference,
	/** A temporary object reference. */
	kRlcReferenceTypeTempReference
};


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
	/** Expected dynamic qualifier. */
	kRlcTypeIndirectionExpectDynamic,
	/** Maybe dynamic qualifier. */
	kRlcTypeIndirectionMaybeDynamic,
	/** Future qualifier. */
	kRlcTypeIndirectionFuture,

	RLC_ENUM_END(RlcTypeIndirection)
};

/** Parses a type indirection.
@memberof RlcParserData
@param[out] out:
	The indirection to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether any tokens were consumed. */
int rlc_type_indirection_parse(
	enum RlcTypeIndirection * out,
	struct RlcParser * parser);

/** CV qualifier flag enum. */
enum RlcTypeQualifier
{
	/** No qualifier. */
	kRlcTypeQualifierNone = 0,
	/** Const qualifier. */
	kRlcTypeQualifierConst = 1,
	/** Volatile qualifier. */
	kRlcTypeQualifierVolatile = 2,

	RLC_ENUM_END(RlcTypeQualifier)
};

/** Parses a type qualifier.
@memberof RlcParserData
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[out] out:
	The type qualifier to parse.
	@dassert @nonnull
@return
	Whether any tokens were consumed. */
int rlc_type_qualifier_parse(
	enum RlcTypeQualifier * out,
	struct RlcParser * parser);

/** A type modifier that consists of an indirection and a cv qualifier. */
struct RlcTypeModifier
{
	/** The type indirection. */
	enum RlcTypeIndirection fTypeIndirection;
	/** The cv qualifier. */
	enum RlcTypeQualifier fTypeQualifier;
	/** Whether it is an array type. */
	int fIsArray;
	/** The optional array dimensions. */
	struct RlcParsedExpression * fArraySize;
};

/** Parses a type modifier.
@memberof RlcParserData
@relates RlcTypeModifier
@param[out] out:
	The type modifier to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull */
_Nodiscard int rlc_type_modifier_parse(
	struct RlcTypeModifier * out,
	struct RlcParser * parser,
	int expect_indirection);

void rlc_type_modifier_destroy(
	struct RlcTypeModifier * this);

/** Controls what value the type name has. */
enum RlcParsedTypeNameValue
{
	/** The type name has a void type. */
	kRlcParsedTypeNameValueVoid,
	/** The type name is a name. */
	kRlcParsedTypeNameValueName,
	/** Symbol constant type. */
	kRlcParsedTypeNameValueSymbolConstant,
	/** The type name is a function signature. */
	kRlcParsedTypeNameValueFunction,
	/** The type name is the type of an expression. */
	kRlcParsedTypeNameValueExpression,
	/** The type name is a tuple of types. */
	kRlcParsedTypeNameValueTuple
};

struct RlcParsedTupleType
{
	struct RlcParsedTypeName * fTypes;
	RlcSrcSize fTypeCount;
};

/** A (possibly) scope-qualified identifier, or void, and type qualifiers. */
struct RlcParsedTypeName
{
	/** Determines the type name's value type. */
	enum RlcParsedTypeNameValue fValue;
	union {
		/** The name of the type. */
		struct RlcParsedSymbol * fName;
		/** The symbol constant name. */
		struct RlcSrcString fSymbolConstant;
		/** The function signature type. */
		struct RlcParsedFunctionSignature * fFunction;
		/** The expression (for type-of). */
		struct RlcParsedExpression * fExpression;
		/** Tuple types. */
		struct RlcParsedTupleType fTuple;
	};

	int fNoDecay;
	int fVariadicExpand;

	/** The type modifiers list. */
	struct RlcTypeModifier * fTypeModifiers;
	/** The type modifier count. */
	size_t fTypeModifierCount;
	/** Whether the type is a reference. */
	enum RlcReferenceType fReferenceType;
};


/** Destroys a type name.
	Releases memory allocated by the given type name, but not the type name itself.
@memberof RlcParsedTypeName
@param[in] this:
	The type name to destroy.
	@dassert @nonnull */
void rlc_parsed_type_name_destroy(
	struct RlcParsedTypeName * this);
/** Adds a type modifier to the type name.
@memberof RlcParsedTypeName
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
@memberof RlcParsedTypeName
@param[in,out] this:
	The parsed type name to create.
	@dassert @nonnull */
void rlc_parsed_type_name_create(
	struct RlcParsedTypeName * this);

/** Parses a type name.
@memberof RlcParsedTypeName
@param[out] out:
	The type name to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success, 0 on error. */
int rlc_parsed_type_name_parse(
	struct RlcParsedTypeName * out,
	struct RlcParser * parser,
	int allow_reference);

/** Retrieves a type name's top level modifier.
@param[in] this:
	The type name to inspect.
	@dassert @nonnull
@return
	The type's top level modifier (does not resolve types). */
struct RlcTypeModifier const * rlc_parsed_type_name_top_modifier(
	struct RlcParsedTypeName const * this);

void rlc_parsed_type_name_print(
	struct RlcParsedTypeName const * this,
	struct RlcSrcFile const * file,
	FILE * out);


/** A function signature. */
struct RlcParsedFunctionSignature
{
	/** Whether the function is a closure. */
	int fIsClosure;
	/** Whether the function is asynchronous. */
	int fIsAsync;
	/** The argument types. */
	struct RlcParsedTypeName * fArguments;
	/** The argument count. */
	RlcSrcSize fArgumentCount;
	/** The result type. */
	struct RlcParsedTypeName fResult;
};

/** Creates a function signature.
@memberof RlcParsedFunctionSignature
@param[out] this:
	The function signature to create.
	@dassert @nonnull */
void rlc_parsed_function_signature_create(
	struct RlcParsedFunctionSignature * this);

/** Destroys a function signature.
@memberof RlcParsedFunctionSignature
@param[in,out] this:
	The function signature to destroy.
	@dassert @nonnull */
void rlc_parsed_function_signature_destroy(
	struct RlcParsedFunctionSignature * this);

/** Adds an argument to a function signature.
@memberof RlcParsedFunctionSignature
@param[in,out] this:
	The function signature to add an argument to.
	@dassert @nonnull
@param[in,out] argument:
	The argument to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_function_signature_add_argument(
	struct RlcParsedFunctionSignature * this,
	struct RlcParsedTypeName * argument);

/** Parses a function signature.
@memberof RlcParsedFunctionSignature
@param[out] out:
	The function signature to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success, 0 on error. */
int rlc_parsed_function_signature_parse(
	struct RlcParsedFunctionSignature * out,
	struct RlcParser * parser);

void rlc_parsed_function_signature_print(
	struct RlcParsedFunctionSignature const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif