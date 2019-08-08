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

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** A function as used in the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedFunction
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** Whether the function has an explicit return type. */
	int fHasReturnType;
	/** The return type of the function. */
	struct RlcParsedTypeName fReturnType;

	/** The arguments the function takes. */
	struct RlcParsedVariable * fArguments;
	/** The argument count. */
	size_t fArgumentCount;

	/** The template arguments. */
	struct RlcTemplateDecl fTemplates;

	/** Whether the function is inline. */
	int fIsInline;
	/** Whether the function is asynchronous. */
	int fIsAsync;

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
	size_t start_index);

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
int rlc_parsed_function_parse(
	struct RlcParsedFunction * out,
	struct RlcParserData * parser);

/** Member function type used by the parser.
@extends RlcParsedMember
@implements RlcParsedFunction */
struct RlcParsedMemberFunction
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedFunction);

	enum RlcMemberAttribute fAttribute;
};

/** Creates a member function.
@memberof RlcParsedMemberFunction
@param[out] this:
	The member function to create.
	@dassert @nonnull */
void rlc_parsed_member_function_create(
	struct RlcParsedMemberFunction * this,
	enum RlcVisibility visiblity,
	size_t start_index);

/** Destroys a parsed member function.
@memberof RlcParsedMemberFunction
@param[in] this:
	The parsed member function to destroy.
	@dassert @nonnull */
void rlc_parsed_member_function_destroy(
	struct RlcParsedMemberFunction * this);

int rlc_parsed_member_function_parse(
	struct RlcParsedMemberFunction * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif