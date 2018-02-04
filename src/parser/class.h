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
	struct RlcTemplateDecl fTemplateDecl;

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
@param[in] start_index:
	The starting index in the token stream. */
void rlc_parsed_class_create(
	struct RlcParsedClass * this,
	size_t start_index);

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
@return
	Nonzero on success. */
int rlc_parsed_class_parse(
	struct RlcParsedClass * out,
	struct RlcParserData * parser);

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
@param[in] start_index:
	The start index of the class.
@param[in] visibility:
	The visibility level of the member class.*/
void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	size_t start_index,
	enum RlcVisibility visibility);

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
@param[in,out] default_visibility:
	The default visibility to use.
@return
	Nonzero on success. */
int rlc_parsed_member_class_parse(
	struct RlcParsedMemberClass * this,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif