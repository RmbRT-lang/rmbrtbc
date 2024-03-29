/** @file function.h
	Defines the function structure used by the parser. */
#ifndef __rlc_parser_function_h_defined
#define __rlc_parser_function_h_defined

#include "scopeentry.h"
#include "typename.h"
#include "variable.h"
#include "member.h"
#include "blockstatement.h"
#include "templatedecl.h"
#include "operatorexpression.h"

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

enum RlcFunctionReturnType
{
	// C++ requires us not to print anything as a type (casts etc.)
	kRlcFunctionReturnTypeNone,
	kRlcFunctionReturnTypeAuto,
	kRlcFunctionReturnTypeType
};

enum RlcFunctionType
{
	kRlcFunctionTypeFunction,
	kRlcFunctionTypeOperator,
	kRlcFunctionTypeCast,
	kRlcFunctionTypeFactory
};

/** A function as used in the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedFunction
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	enum RlcFunctionType fType;
	enum RlcOperator fOperatorName;

	/** Whether the function has an explicit return type. */
	enum RlcFunctionReturnType fHasReturnType;
	/** The return type of the function. */
	struct RlcParsedTypeName fReturnType;
	enum RlcTypeQualifier fAutoReturnQualifier;
	enum RlcReferenceType fAutoReturnReference;

	/** The arguments the function takes. */
	struct RlcParsedVariable * fArguments;
	/** The argument count. */
	size_t fArgumentCount;

	/** The template arguments. */
	struct RlcParsedTemplateDecl fTemplates;

	/** Whether the function is inline. */
	int fIsInline;
	/** Whether the function is asynchronous. */
	int fIsAsync;

	/** Whether the function has a body (i.e., declarations). */
	int fHasBody;

	union {
		/** The function's return value expression. */
		struct RlcParsedExpression * fReturnValue;
		/** The function's body. */
		struct RlcParsedBlockStatement fBodyStatement;
	};

	/** If this is zero, `fBodyStatement` is used, otherwise `fReturnValue`. */
	int fIsShortHandBody;
};

/** Creates an empty function.
@memberof RlcParsedFunction
@param[in] this:
	The the function to initialise.
	@dassert @nonnull */
void rlc_parsed_function_create(
	struct RlcParsedFunction * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates);

/** Destroys a parsed function.
@memberof RlcParsedFunction
@param[in] this:
	The function to destroy. @dassert @nonnull */
void rlc_parsed_function_destroy(
	struct RlcParsedFunction * this);

/** Adds an argument to a function.
@memberof RlcParsedFunction
@param[in,out] this:
	The function to add an argument to.
	@dassert @nonnull
@param[in,out] variable:
	The argument to add.
	@instance_ownership
	@dassert @nonnull */
void rlc_parsed_function_add_argument(
	struct RlcParsedFunction * this,
	struct RlcParsedVariable * variable);

/** Parses a function.
@memberof RlcParsedFunction
@param[out] function:
	The function to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_function_parse(
	struct RlcParsedFunction * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates,
	int allow_body);

void rlc_parsed_function_print(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * out);

void rlc_parsed_function_print_head(
	struct RlcParsedFunction const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	int templates);

enum RlcMemberFunctionAbstractness
{
	kRlcMemberFunctionAbstractnessNone,
	kRlcMemberFunctionAbstractnessVirtual,
	kRlcMemberFunctionAbstractnessAbstract,
	kRlcMemberFunctionAbstractnessOverride,
	kRlcMemberFunctionAbstractnessFinal
};

/** Member function type used by the parser.
@extends RlcParsedMember
@implements RlcParsedFunction */
struct RlcParsedMemberFunction
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedFunction);

	/** Whether the member function is virtual. */
	enum RlcMemberFunctionAbstractness fAbstractness;
};

/** Creates a member function.
@memberof RlcParsedMemberFunction
@param[out] this:
	The member function to create.
	@dassert @nonnull */
void rlc_parsed_member_function_create(
	struct RlcParsedMemberFunction * this,
	struct RlcParsedMemberCommon const * member);

/** Destroys a parsed member function.
@memberof RlcParsedMemberFunction
@param[in] this:
	The parsed member function to destroy.
	@dassert @nonnull */
void rlc_parsed_member_function_destroy(
	struct RlcParsedMemberFunction * this);

_Nodiscard int rlc_parsed_member_function_parse(
	struct RlcParsedMemberFunction * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member);

void rlc_parsed_member_function_print(
	struct RlcParsedMemberFunction const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * out);

#ifdef __cplusplus
}
#endif

#endif