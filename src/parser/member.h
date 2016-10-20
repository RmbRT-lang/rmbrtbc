/** @file member.h
	Contains the type that stores member information used by the parser. */
#ifndef __rlc_parser_member_h_defined
#define __rlc_parser_member_h_defined

#include "../macros.h"

#include <stddef.h>

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
	/** Corresponds to `RlcParsedMemberClass`. */
	kRlcParsedMemberClass,

	RLC_ENUM_END(RlcParsedMemberType)
};


/** Contains information of member declarations. */
struct RlcParsedMember
{
	RLC_ABSTRACT(RlcParsedMember);
	
	/** The visibility level of the member. */
	enum RlcVisibility fVisibility;
};

/** Creates a member.
@memberof RlcParsedMember
@param[out] this:
	The member to create.
	@dassert @nonnull
@param[in] type:
	The deriving type.
@param[in] visibility:
	The visibility level of the member.
@param[in] isStatic:
	Whether the member is static. */
void rlc_parsed_member_create(
	struct RlcParsedMember * this,
	enum RlcParsedMemberType type,
	enum RlcVisibility visibility);

/** Destroys a parsed member.
@memberof RlcParsedMember
@param[in] this:
	The member to destroy.
	@dassert @nonnull */
void rlc_parsed_member_destroy_virtual(
	struct RlcParsedMember * this);

/** Destroys a parsed member.
	This function is to be called by deriving destructors.
@memberof RlcParsedMember
@param[in] this:
	The member to destroy.
	@dassert @nonnull */
inline void rlc_parsed_member_destroy_base(
	struct RlcParsedMember * this) { }

/** A list of parsed members. */
struct RlcParsedMemberList
{
	/** The member list. */
	struct RlcParsedMember * * fEntries;
	/** The member count. */
	size_t fEntryCount;
};

/** Creates a member list.
@memberof RlcParsedMemberList
@param[out] this:
	The member list to create.
	@dassert @nonnull */
void rlc_parsed_member_list_create(
	struct RlcParsedMemberList * this);

/** Adds a member to a member list.
@memberof RlcParsedMemberList
@param[in,out] this:
	The member list to add a member to.
	@dassert @nonnull
@param[in] member:
	The member to add to the list.
	@pass_pointer_ownership
	@dassert @nonnull */
void rlc_parsed_member_list_add(
	struct RlcParsedMemberList * this,
	struct RlcParsedMember * member);

/** Destroys a member list.
@param[in,out] this:
	The member list to destroy. */
void rlc_parsed_member_list_destroy(
	struct RlcParsedMemberList * this);

#ifdef __cplusplus
}
#endif

#endif