/** @file class.h
	Contains the definition of the class type used by the parser. */
#ifndef __rlc_parser_class_h_defined
#define __rlc_parser_class_h_defined

#include "destructor.h"
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
	struct RlcParsedTemplateDecl fTemplateDecl;

	/** The list of members. */
	struct RlcParsedMemberList fMembers;

	/** Whether the class has a destructor. */
	int fHasDestructor;
	/** The class' destructor. */
	struct RlcParsedDestructor fDestructor;
};

/** Creates a parsed class.
@memberof RlcParsedClass
@param[out] this:
	The parsed class to create.
	@dassert @nonnull
@param[in] name:
	The class's name.
	@dassert @nonnull
@param[in] templates:
	The class's templates. */
void rlc_parsed_class_create(
	struct RlcParsedClass * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates);

/** Destroys a parsed class.
@memberof RlcParsedClass
@param[in,out] this:
	The parsed class to destroy.
	@dassert @nonnull */
void rlc_parsed_class_destroy(
	struct RlcParsedClass * this);

/** Tries to parse a class.
@memberof RlcParsedClass
@param[out] out:
	The class to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[in] templates:
	The class's templates.
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_class_parse(
	struct RlcParsedClass * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

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
@param[in] member:
	The common part of the member.
	@dassert @nonnull */
void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	struct RlcParsedMemberCommon const * member);

/** Destroys a member class.
@memberof RlcParsedMemberClass
@param[in,out] this:
	The member class to destroy.
	@dassert @nonnull */
void rlc_parsed_member_class_destroy(
	struct RlcParsedMemberClass * this);

/** Parses a member class.
@memberof RlcParsedMemberClass
@param[out] this:
	The member class to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[in] member:
	The member values.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_member_class_parse(
	struct RlcParsedMemberClass * this,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member);

#ifdef __cplusplus
}
#endif

#endif