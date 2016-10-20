/** @file rawtype.h
	Contains the rawtype class. */
#ifndef __rlc_parser_rawtype_h_defined
#define __rlc_parser_rawtype_h_defined

#include "scopeentry.h"
#include "member.h"

#include "expression.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Rawtype class. */
struct RlcParsedRawtype
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The rawtype's size expression. */
	struct RlcParsedExpression * fSize;
	/** The rawtype's member list. */
	struct RlcParsedMemberList fMembers;
};

/** Creates a parsed rawtype.
@memberof RlcParsedRawtype
@param[out] this:
	The parsed rawtype to create.
	@dassert @nonnull */
void rlc_parsed_rawtype_create(
	struct RlcParsedRawtype * this);

/** Destroys a parsed rawtype.
@memberof RlcParsedRawtype
@param[in,out] this:
	The rawtype to destroy.
	@dassert @nonnull */
void rlc_parsed_rawtype_destroy(
	struct RlcParsedRawtype * this);


#ifdef __cplusplus
}
#endif

#endif