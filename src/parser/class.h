/** @file class.h
	Contains the definition of the class type used by the parser. */
#ifndef __rlc_parser_class_h_defined
#define __rlc_parser_class_h_defined

#include "scopeentry.h"
#include "templatedecl.h"
#include "member.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The class type as used by the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedClass
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The template declarations of the class. */
	struct RlcTemplateDecl fTemplateDecl;

	/** The list of members. */
	struct RlcParsedMemberList fMembers;
};

/** Creates a parsed class.
@memberof RlcParsedClass
@param[out] this:
	The parsed class to create.
	@dassert @nonnull */
void rlc_parsed_class_create(
	struct RlcParsedClass * this);

/** Destroys a parsed class.
@memberof RlcParsedClass
@param[in,out] this:
	The parsed class to destroy.
	@dassert @nonnull */
void rlc_parsed_class_destroy(
	struct RlcParsedClass * this);

/** The member class type as used by the parser.
@extends RlcParsedClass
@extends RlcParsedMember */
struct RlcParsedMemberClass
{
	RLC_DERIVE(struct,RlcParsedClass);
	RLC_DERIVE(struct,RlcParsedMember);
};

/** Creates a member class.
@memberof RlcParsedMemberClass
@param[out] this:
	The member class to create.
	@dassert @nonnull
@param[in] visibility:
	The visibility level of the member class.*/
void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	enum RlcVisibility visibility);

/** Destroys a member class.
@memberof RlcParsedMemberClass
@param[in,out] this:
	The member class to destroy.
	@dassert @nonnull */
void rlc_parsed_member_class_destroy(
	struct RlcParsedMemberClass * this);


#ifdef __cplusplus
}
#endif

#endif