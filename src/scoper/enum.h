#ifndef __rlc_scoper_enum_h_defined
#define __rlc_scoper_enum_h_defined

#include "../src/string.h"
#include "../parser/enum.h"
#include "scopeentry.h"
#include "member.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedMemberEnum;

/** A scoped enum constant. */
struct RlcScopedEnumConstant
{
	RLC_DERIVE(struct, RlcScopedScopeEntry);
	/** The enum constant's numeric value. */
	RlcSrcIndex fValue;
	/** The enum type. */
	struct RlcScopedEnum * fType;
};

enum RlcScopedEnumType
{
	kRlcScopedGlobalEnum,
	kRlcScopedMemberEnum,
	RLC_ENUM_END(RlcScopedEnumType)
};

/** A scoped enum. */
struct RlcScopedEnum
{
	RLC_ABSTRACT(RlcScopedEnum);
	/** The amount of unique enum values. */
	RlcSrcSize fSize;
};

/** Scoped global enum type.
@extends RlcScopedScopeEntry
@extends RlcScopedEnum */
struct RlcScopedGlobalEnum
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
	RLC_DERIVE(struct,RlcScopedEnum);
};

/** Scoped member enum type.
@extends RlcScopedMember
@extends RlcScopedEnum */
struct RlcScopedMemberEnum
{
	RLC_DERIVE(struct,RlcScopedMember);
	RLC_DERIVE(struct,RlcScopedEnum);
};

/** Creates a scoped enum from a parsed enum.
@memberof RlcScopedEnum
@param[out] this:
	The enum to create.
	@dassert @nonnull
@param[in] file:
	The parsed enum's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed enum.
	@dassert @nonnull */
void rlc_scoped_global_enum_create(
	struct RlcScopedGlobalEnum * this,
	struct RlcSrcFile const * file,
	struct RlcParsedEnum const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped enum.
@memberof RlcScopedEnum
@param[in,out] this:
	The enum to destroy.
	@dassert @nonnull */
void rlc_scoped_global_enum_destroy(
	struct RlcScopedGlobalEnum * this);

/** Creates a scoped member enum from a parsed member enum.
@memberof RlcScopedMemberEnum
@param[out] this:
	The member enum to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed member enum.
	@dassert @nonnull
@param[in,out] parent:
	The parent scope.
	@dassert @nonnull */
void rlc_scoped_member_enum_create(
	struct RlcScopedMemberEnum * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberEnum * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped member enum.
@memberof RlcScopedMemberEnum
@param[in,out] this:
	The member enum to destroy.
	@dassert @nonnull */
void rlc_scoped_member_enum_destroy(
	struct RlcScopedMemberEnum * this);

/** Creates a new scoped enum constant.
@memberof RlcScopedEnumConstant
@param[in] file:
	The source file the enum constant is in.
	@dassert @nonnull
@param[in] parsed:
	The parsed enum constant.
	@dassert @nonnull*/
struct RlcScopedEnumConstant * rlc_scoped_enum_constant_new(
	struct RlcSrcFile const * file,
	struct RlcParsedEnumConstant const * parsed,
	RlcSrcIndex value,
	struct RlcScopedScopeItemGroup * group,
	struct RlcScopedEnum * parent);

/** Destroys a scoped enum constant.
@memberof RlcScopedEnumConstant
@param[in,out] this:
	The enum constant to destroy.
	@dassert @nonnull */
void rlc_scoped_enum_constant_destroy(
	struct RlcScopedEnumConstant * this);

#ifdef __cplusplus
}
#endif


#endif