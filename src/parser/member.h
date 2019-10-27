/** @file member.h
	Contains the type that stores member information used by the parser. */
#ifndef __rlc_parser_member_h_defined
#define __rlc_parser_member_h_defined

#include "parser.h"
#include "templatedecl.h"

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
	/** Corresponds to `RlcParsedMemberRawtype`. */
	kRlcParsedMemberRawtype,
	/** Corresponds to `RlcParsedMemberStruct`. */
	kRlcParsedMemberStruct,
	/** Corresponds to `RlcParsedMemberUnion`. */
	kRlcParsedMemberUnion,
	/** Corresponds to `RlcParsedMemberClass`. */
	kRlcParsedMemberClass,

	/** Corresponds to `RlcParsedMemberTypedef`. */
	kRlcParsedMemberTypedef,

	/** Corresponds to `RlcParsedConstructor`. */
	kRlcParsedConstructor,
	/** Corresponds to `RlcParsedDestructor`.*/
	kRlcParsedDestructor,

	RLC_ENUM_END(RlcParsedMemberType)
};

enum RlcMemberAttribute
{
	kRlcMemberAttributeNone,
	kRlcMemberAttributeIsolated,
	kRlcMemberAttributeStatic,

	RLC_ENUM_END(RlcMemberAttribute)
};

/** Parses a member attribute.
@memberof RlcMemberAttribute
@param[in] parser:
	The parser data.
	@dassert @nonnull
@return
	The parsed member attribute. */
enum RlcMemberAttribute rlc_member_attribute_parse(
	struct RlcParser * parser);

/** Contains information of member declarations. */
struct RlcParsedMember
{
	RLC_ABSTRACT(RlcParsedMember);

	/** The visibility level of the member. */
	enum RlcVisibility fVisibility;
	/** The member's attribute. */
	enum RlcMemberAttribute fAttribute;
};

/** Common fields that are passed to member parsing functions. */
struct RlcParsedMemberCommon
{
	/** The member's visibility. */
	enum RlcVisibility visibility;
	/** The member's default visibility. */
	enum RlcVisibility default_visibility;
	/** The member's attribute. */
	enum RlcMemberAttribute attribute;
	/** The member's templates. */
	struct RlcParsedTemplateDecl templates;
};

void rlc_parsed_member_common_create(
	struct RlcParsedMemberCommon * this,
	enum RlcVisibility default_visibility);

/** Parses the common fields of a member.
@param[out] out:
	The common member fields to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser.
	@dassert @nonnull
@return
	Whether a member needs to follow. */
_Nodiscard int rlc_parsed_member_common_parse(
	struct RlcParsedMemberCommon * out,
	struct RlcParser * parser);

/** Parses a visibility modifier. */
enum RlcVisibility rlc_visibility_parse(
	enum RlcVisibility * default_visibility,
	struct RlcParser * parser);

/** Creates a member.
@memberof RlcParsedMember
@param[out] this:
	The member to create.
	@dassert @nonnull
@param[in] type:
	The deriving type.
@param[in] common:
	The member's common fields. */
void rlc_parsed_member_create(
	struct RlcParsedMember * this,
	enum RlcParsedMemberType type,
	struct RlcParsedMemberCommon const * common);

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
void rlc_parsed_member_destroy_base(
	struct RlcParsedMember * this);

/** Returns the name of a parsed member.
@memberof RlcParsedMember
@param[in] this:
	The member whose name to retrieve.
	@dassert @nonnull
@return
	The member's name. `NULL` if the member has no name (i.e., constructors or destructors). */
struct RlcSrcString const * rlc_parsed_member_name(
	struct RlcParsedMember * this);

/** Parses a member declaration.
	Goes through all member types and tries to parse them.
@memberof RlcParsedMember
@param[in,out] parser:
	The parser data.
@param[in,out] member:
	The common member attributes. Use the same value across multiple calls to track the visibility of the members.
@param[in] flags:
	The flags for the RlcParsedMemberType values. If set, a corresponding RlcParsedMember can be parsed, otherwise, is ignored.
@return
	`NULL` if no member declaration could be parsed, otherwise a pointer to a dynamically allocated instance of the deriving type. */
struct RlcParsedMember * rlc_parsed_member_parse(
	struct RlcParser * parser,
	struct RlcParsedMemberCommon * member,
	int flags);

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