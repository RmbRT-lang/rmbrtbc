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
	size_t start_index);

/** Destroys a parsed union.
@memberof RlcParsedUnion
@param[in] this:
	The union to destroy.*/
void rlc_parsed_union_destroy(
	struct RlcParsedUnion * this);

int rlc_parsed_union_parse(
	struct RlcParsedUnion * out,
	struct RlcParserData * parser);

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
	enum RlcVisibility visibility,
	size_t start_index);

void rlc_parsed_member_union_destroy(
	struct RlcParsedMemberUnion * this);

int rlc_parsed_member_union_parse(
	struct RlcParsedMemberUnion * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif