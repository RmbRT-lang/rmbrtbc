/** @file enum.h
	Contains the enum struct as used by the parser. */
#ifndef __rlc_parser_enum_h_defined
#define __rlc_parser_enum_h_defined

#include "parser.h"
#include "scopeentry.h"
#include "member.h"
#include "templatedecl.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Holds a single constant of an enum, with an arbitrary number of aliases.
@see `RlcParsedEnum`. */
struct RlcParsedEnumConstant
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The aliases. */
	struct RlcSrcString * fAliasTokens;
	/** The alias count. */
	size_t fAliasCount;
};

/** Creates an enum constant.
@param[out] this:
	The enum constant to create. */
void rlc_parsed_enum_constant_create(
	struct RlcParsedEnumConstant * this,
	struct RlcSrcString const * name);

/** Adds an alias to an enum constant.
@param[in,out] this:
	The enum constant to add a name to.
	@dassert @nonnull
@param[in] name:
	The alias. */
void rlc_parsed_enum_constant_add_name(
	struct RlcParsedEnumConstant * this,
	struct RlcSrcString const * name);

/** Destroys an enum constant.
@param[in,out] this:
	The constant to destroy. */
void rlc_parsed_enum_constant_destroy(
	struct RlcParsedEnumConstant * this);

/** Parses an enum constant.
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[out] out:
	The enum constant. */
_Nodiscard int rlc_parsed_enum_constant_parse(
	struct RlcParsedEnumConstant * out,
	struct RlcParser * parser);



/** Enum type.
@extends RlcParsedScopeEntry */
struct RlcParsedEnum
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The constants. */
	struct RlcParsedEnumConstant * fConstants;
	/** The constant count. */
	size_t fConstantCount;
};

void rlc_parsed_enum_print(
	struct RlcParsedEnum const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer);

/** Member enum type.
@extends RlcParsedMember
@extends RlcParsedEnum */
struct RlcParsedMemberEnum
{
	RLC_DERIVE(struct,RlcParsedMember);
	RLC_DERIVE(struct,RlcParsedEnum);
};

/** Creates an enum.
@param[out] this:
	The enum to create.
	@dassert @nonnull
@param[in] name:
	The enum's name.
	@dassert @nonnull */
void rlc_parsed_enum_create(
	struct RlcParsedEnum * this,
	struct RlcSrcString const * name);

/** Destroys an enum.
@memberof RlcParsedEnum
@param[in,out] this:
	The enum to destroy. */
void rlc_parsed_enum_destroy(
	struct RlcParsedEnum * this);

/** Adds a constant to an enum.
@param[in,out] this:
	The enum to add a constant to.
@param[in] constant:
	The constant to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_enum_add_constant(
	struct RlcParsedEnum * this,
	struct RlcParsedEnumConstant const * constant);

/** Parses an enum.
@param[in,out] parser:
	The parser.
	@dassert @nonnull
@param[out] out:
	The enum to parse.
	@dassert @nonnull */
_Nodiscard int rlc_parsed_enum_parse(
	struct RlcParsedEnum * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);


/** Creates a member enum.
@param[out] this:
	The member enum to create.
	@dassert @nonnull
@param[in] member:
	The common part of the member.
	@dassert @nonnull */
void rlc_parsed_member_enum_create(
	struct RlcParsedMemberEnum * this,
	struct RlcParsedMemberCommon const * member);

/** Destroys a member enum.
@param[in,out] this:
	The member enum to destroy.
	@dassert @nonnull */
void rlc_parsed_member_enum_destroy(
	struct RlcParsedMemberEnum * this);

/** Parses a member enum.
@param[out] out:
	The member enum to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser.
	@dassert @nonnull
@param[in] member:
	The member values.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_member_enum_parse(
	struct RlcParsedMemberEnum * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member);

void rlc_parsed_member_enum_print(
	struct RlcParsedMemberEnum const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer);

#ifdef __cplusplus
}
#endif


#endif