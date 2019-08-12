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
	size_t start_index);

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
int rlc_parsed_rawtype_parse(
	struct RlcParsedRawtype * out,
	struct RlcParserData * parser);

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
	enum RlcVisibility visibility,
	size_t start_index);

void rlc_parsed_member_rawtype_destroy(
	struct RlcParsedMemberRawtype * this);

int rlc_parsed_member_rawtype_parse(
	struct RlcParsedMemberRawtype * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser);


#ifdef __cplusplus
}
#endif

#endif