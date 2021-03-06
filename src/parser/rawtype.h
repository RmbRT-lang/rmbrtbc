/** @file rawtype.h
	Contains the rawtype class. */
#ifndef __rlc_parser_rawtype_h_defined
#define __rlc_parser_rawtype_h_defined

#include "scopeentry.h"
#include "member.h"
#include "expression.h"
#include "templatedecl.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Rawtype class.
@extends RlcParsedScopeEntry */
struct RlcParsedRawtype
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The rawtype's size expression. */
	struct RlcParsedExpression * fSize;

	/** The rawtype's template arguments. */
	struct RlcParsedTemplateDecl fTemplates;

	/** The rawtype's member list. */
	struct RlcParsedMemberList fMembers;
};

/** Creates a parsed rawtype.
@memberof RlcParsedRawtype
@param[out] this:
	The parsed rawtype to create.
	@dassert @nonnull */
void rlc_parsed_rawtype_create(
	struct RlcParsedRawtype * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates);

/** Destroys a parsed rawtype.
@memberof RlcParsedRawtype
@param[in,out] this:
	The rawtype to destroy.
	@dassert @nonnull */
void rlc_parsed_rawtype_destroy(
	struct RlcParsedRawtype * this);

/** Parses a rawtype.
@memberof RlcParsedRawtype
@param[out] out:
	The rawtype to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_rawtype_parse(
	struct RlcParsedRawtype * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

void rlc_parsed_rawtype_print(
	struct RlcParsedRawtype const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer);

/** A member rawtype as used by the parser.
@implements RlcParsedMember
@implements RlcParsedRawtype */
struct RlcParsedMemberRawtype
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedRawtype);
};

void rlc_parsed_member_rawtype_create(
	struct RlcParsedMemberRawtype * this,
	struct RlcParsedMemberCommon const * member);

void rlc_parsed_member_rawtype_destroy(
	struct RlcParsedMemberRawtype * this);

_Nodiscard int rlc_parsed_member_rawtype_parse(
	struct RlcParsedMemberRawtype * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member);


#ifdef __cplusplus
}
#endif

#endif