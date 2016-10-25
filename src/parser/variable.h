/** @file variable.h
	Defines the variable struct. */
#ifndef __rlc_parser_variable_h_defined
#define __rlc_parser_variable_h_defined

#include "scopeentry.h"
#include "expression.h"
#include "typename.h"
#include "member.h"
#include "parser.h"
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

/** Parses a variable.
@memberof RlcParsedVariable
@param[out] out:
	The variable to parse.
@param[in,out] parser:
	The parser data.
@return
	Nonzero on success. */
int rlc_parsed_variable_parse(
	struct RlcParsedVariable * out,
	struct RlcParserData * parser,
	int allow_initialiser);

/** Member variable type.
@extends RlcParsedMember
@extends RlcParsedVariable */
struct RlcParsedMemberVariable
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedVariable);
	int fIsolated;
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

/** Parses a member variable, but does not parse the `RlcParsedMember` base instance.
	To completely parse it, call `rlc_parsed_member_parse_base()` beforehand on a seperate `RlcParsedMember` instance and assign it to `out` afterwards.
@memberof RlcParsedMemberVariable
@param[out] out:
	The member variable to parse into. The `RlcParsedMember` will not be parsed.
@param[in,out] parser:
	The parser data.
@return
	Whether the parsing was successful. */
int rlc_parsed_member_variable_parse(
	struct RlcParsedMemberVariable * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif