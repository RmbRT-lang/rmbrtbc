/** @file typedef.h
	Contains the type for type aliasing used by the parser. */
#ifndef __rlc_parser_typedef_h_defined
#define __rlc_parser_typedef_h_defined

#include "parser.h"
#include "typename.h"
#include "scopeentry.h"
#include "member.h"
#include "templatedecl.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Describes a typedef as used by the parser.
@implements RlcParsedScopeEntry */
struct RlcParsedTypedef
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The template declaration. */
	struct RlcParsedTemplateDecl fTemplates;
	/** The type name that has been aliased. */
	struct RlcParsedTypeName fType;
};

void rlc_parsed_typedef_create(
	struct RlcParsedTypedef * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates);

/** Destroys a typedef.
@memberof RlcParsedTypedef
@param[out] this:
	The typedef to destroy. */
void rlc_parsed_typedef_destroy(
	struct RlcParsedTypedef * this);

/** Parses a typedef.
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[out] out:
	The typedef to parse into.
	@dassert @nonnull */
_Nodiscard int rlc_parsed_typedef_parse(
	struct RlcParsedTypedef * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

/** Describes a member typedef as used by the parser.
@extends RlcParsedTypedef
@extends RlcParsedMember */
struct RlcParsedMemberTypedef
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedTypedef);
};

/** Creates a member typedef.
@memberof RlcParsedMemberTypedef
@param[out] this:
	The member typedef to create.
	@dassert @nonnull
@param[in,out] visibility:
	The visibility to use.
@param[in] start_index:
	The starting index. */
void rlc_parsed_member_typedef_create(
	struct RlcParsedMemberTypedef * this,
	struct RlcParsedMemberCommon const * member);

/** Destroys a member typedef.
@memberof RlcParsedMemberTypedef
@param[in,out] this:
	The member typedef to destroy.
	@dassert @nonnull. */
void rlc_parsed_member_typedef_destroy(
	struct RlcParsedMemberTypedef * this);

/** Parses a member typedef.
@memberof RlcParsedMemberTypedef */
_Nodiscard int rlc_parsed_member_typedef_parse(
	struct RlcParsedMemberTypedef * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * common);

#ifdef __cplusplus
}
#endif

#endif