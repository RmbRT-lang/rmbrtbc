/** @file member.h
	Contains the type that stores member information used by the parser. */
#ifndef __rlc_parser_member_h_defined
#define __rlc_parser_member_h_defined

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The protection level of a member. */
enum RlcVisibility
{
	/** Public visibility, visible to all. */
	kRlcVisibilityPublic,
	/** Protected visibility, visible to self and deriving types. */
	kRlcVisibilityProtected,
	/** Private visibility, visible to self. */
	kRlcVisibilityPrivate,

	RLC_ENUM_END(RlcVisibility)
};

/** The deriving type of RlcParsedMember. */
enum RlcParsedMemberType
{
	/** Corresponds to `RlcParsedMemberFunction`. */
	kRlcParsedMemberFunction,
	/** Corresponds to `RlcParsedMemberVariable`. */
	kRlcParsedMemberVariable,

	RLC_ENUM_END(RlcParsedMemberType)
};


/** Contains information of member declarations. */
struct RlcParsedMember
{
	/** The deriving type. */
	enum RlcParsedMemberType fMemberType;
	/** The visibility level of the member. */
	enum RlcVisibility fVisibility;
	/** Whether the member is static. */
	int fIsStatic;
};

/** Destroys a parsed member.
@memberof RlcParsedMember
@param[in] this:
	The member to destroy.
	@dassert @nonnull */
void rlc_parsed_member_destroy(
	struct RlcParsedMember * this);

#ifdef __cplusplus
}
#endif

#endif