/** @file union.h
	Contains the union type description. */
#ifndef __rlc_parser_union_h_defined
#define __rlc_parser_union_h_defined

#include "scopeentry.h"
#include "member.h"
#include "templatedecl.h"

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Union types as used by the parser.
@implements RlcParsedScopeEntry */
struct RlcParsedUnion
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The template declaration. */
	struct RlcParsedTemplateDecl fTemplates;
	/** The list of members. */
	struct RlcParsedMemberList fMembers;
};

/** Creates a parsed union.
@memberof RlcParsedUnion
@param[out] this:
	The union to create.
	@dassert @nonnull */
void rlc_parsed_union_create(
	struct RlcParsedUnion * this,
	struct RlcSrcString const * name);

/** Destroys a parsed union.
@memberof RlcParsedUnion
@param[in] this:
	The union to destroy.*/
void rlc_parsed_union_destroy(
	struct RlcParsedUnion * this);

_Nodiscard int rlc_parsed_union_parse(
	struct RlcParsedUnion * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

/** A member union as used by the parser.
@implements RlcParsedMember
@implements RlcParsedUnion */
struct RlcParsedMemberUnion
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedUnion);
};

void rlc_parsed_member_union_create(
	struct RlcParsedMemberUnion * this,
	struct RlcParsedMemberCommon const * common);

void rlc_parsed_member_union_destroy(
	struct RlcParsedMemberUnion * this);

_Nodiscard int rlc_parsed_member_union_parse(
	struct RlcParsedMemberUnion * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member);

#ifdef __cplusplus
}
#endif

#endif