/** @file function.h
	Defines the function structure used by the parser. */
#ifndef __rlc_parser_function_h_defined
#define __rlc_parser_function_h_defined

#include "scopeentry.h"
#include "typename.h"
#include "variable.h"
#include "member.h"

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** A function as used in the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedFunction
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The return type of the function. */
	struct RlcParsedTypeName fReturnType;

	/** The arguments the function takes. */
	struct RlcParsedVariable * fArguments;

	/** Whether the function is inline. */
	int fIsInline;
};

/** Creates an empty function.
@memberof RlcParsedFunction
@param[in] this:
	The the function to initialise.
	@dassert @nonnull */
void rlc_parsed_function_create(
	struct RlcParsedFunction * this);

/** Destroys a parsed function.
@memberof RlcParsedFunction
@param[in] this:
	The function to destroy. @dassert @nonnull */
void rlc_parsed_function_destroy(
	struct RlcParsedFunction * this);

/** Member function type used by the parser.
@extends RlcParsedMember
@implements RlcParsedFunction */
struct RlcParsedMemberFunction
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedFunction);
};

/** Creates a member function.
@memberof RlcParsedMemberFunction
@param[out] this:
	The member function to create.
	@dassert @nonnull */
void rlc_parsed_member_function_create(
	struct RlcParsedMemberFunction * this);

/** Destroys a parsed member function.
@memberof RlcParsedMemberFunction
@param[in] this:
	The parsed member function to destroy.
	@dassert @nonnull */
void rlc_parsed_member_function_destroy(
	struct RlcParsedMemberFunction * this);

#ifdef __cplusplus
}
#endif

#endif