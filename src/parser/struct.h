/** @file struct.h
	Contains the struct type used by the parser. */
#ifndef __rlc_parser_struct_h_defined
#define __rlc_parser_struct_h_defined

#include "scopeentry.h"
#include "member.h"
#include "function.h"
#include "variable.h"
#include "templatedecl.h"

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/** Structure type as used by the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedStruct
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The template declaration. */
	struct RlcParsedTemplateDecl fTemplates;

	/** The struct's members. */
	struct RlcParsedMemberList fMembers;
};

/** Creates a struct.
@memberof RlcParsedStruct
@param[out] this:
	The struct to create.
	@dassert @nonnull */
void rlc_parsed_struct_create(
	struct RlcParsedStruct * this,
	size_t start_index);

/** Destroys a parsed struct.
@memberof RlcParsedStruct
@param[in,out] this:
	The struct to destroy. */
void rlc_parsed_struct_destroy(
	struct RlcParsedStruct * this);
/** Parses a struct.
@memberof RlcParsedStruct
@param[out] out:
	The struct to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data. */
_Nodiscard int rlc_parsed_struct_parse(
	struct RlcParsedStruct * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

/** Member structure type as used by the parser.
@extends RlcParsedMember
@extends RlcParsedStruct */
struct RlcParsedMemberStruct
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedStruct);
};

/** Creates a member struct.
@memberof RlcParsedMemberStruct
@param[out] this:
	The member struct to create.
	@dassert @nonnull */
void rlc_parsed_member_struct_create(
	struct RlcParsedMemberStruct * this,
	enum RlcVisibility visiblity,
	size_t start_index);

/** Destroys a member struct.
@memberof RlcParsedMemberStruct
@param[in,out] this:
	The member struct to destroy.
	@dassert @nonnull */
void rlc_parsed_member_struct_destroy(
	struct RlcParsedMemberStruct * this);

/** Parses a member struct.
@memberof RlcParsedMemberStruct
@param[out] out:
	The member struct to parse.
	@dassert @nonnull
@param[in,out] default_visibility:
	The default visibility level. This value is modified if a block visibility modifier is parsed.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
@return
	Nonzero on success. */
int rlc_parsed_member_struct_parse(
	struct RlcParsedMemberStruct * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);


#ifdef __cplusplus
}
#endif

#endif