/** @file union.h
	Contains the union type description. */
#ifndef __rlc_parser_union_h_defined
#define __rlc_parser_union_h_defined

#include "scopeentry.h"
#include "member.h"

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Union types as used by the parser.
@implements RlcParsedScopeEntry */
struct RlcParsedUnion
{
	/** The scope entry derive*/
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The member functions. */
	struct RlcParsedMemberFunction * fMemberFunctions;
	/** The member function count. */
	size_t fMemberFunctionCount;

	/** The member variables. */
	struct RlcParsedMemberVariable * fMemberVariables;
	/** The member variable count. */
	size_t fMemberVariableCount;
};

/** Destroys a parsed union.
@memberof RlcParsedUnion
@param[in] this:
	The union to destroy.*/
void rlc_parsed_union_destroy(
	struct RlcParsedUnion * this);

/** Searches for a member of a union.
@memberof RlcParsedUnion
@param[in] this:
	The union to search.
@param[in] name:
	The member's name.
@param[in,out] start:
	Where to start searching. Use this to find multiple overloads of functions, for example. The value held will only be valid if nonzero was returned.
	@dassert
		@nonnull
		The referenced pointer shall either be `null` or an address returned by `rlc_parsed_union_find_member()`.
@return
	Whether a member was found. */
int rlc_parsed_union_find_member(
	struct RlcParsedUnion const * this,
	char const * name,
	struct RlcParsedMember const ** start);

#ifdef __cplusplus
}
#endif

#endif