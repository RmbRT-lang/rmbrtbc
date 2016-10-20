/** @file variable.h
	Defines the variable struct. */
#ifndef __rlc_parser_variable_h_defined
#define __rlc_parser_variable_h_defined

#include "scopeentry.h"
#include "expression.h"
#include "typename.h"
#include "member.h"
#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Variables as used in the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedVariable
{
	RLC_DERIVE(struct, RlcParsedScopeEntry);
	
	/** The variable type. */
	struct RlcParsedTypeName fType;
	/** The initialising expression, or null. */
	struct RlcParsedExpression * fInitExp;
};

/** Destroys a variable.
@memberof RlcParsedVariable
@param[in] this:
	The variable to destroy.
	@dassert @nonnull */
void rlc_parsed_variable_destroy(
	struct RlcParsedVariable * this);

/** Member variable type.
@extends RlcParsedMember
@extends RlcParsedVariable */
struct RlcParsedMemberVariable
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedVariable);
};

/** Creates a parsed member variable.
@param[out] this:
	The parsed member variable to create.
	@dassert @nonnull */
void rlc_parsed_member_variable_create(
	struct RlcParsedMemberVariable * this);

/** Destroys a parsed member variable.
@memberof RlcParsedMemberVariable
@param[in] this:
	The parsed member variable to destroy. */
void rlc_parsed_member_variable_destroy(
	struct RlcParsedMemberVariable * this);

#ifdef __cplusplus
}
#endif

#endif