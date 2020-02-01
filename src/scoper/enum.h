#ifndef __rlc_scoper_enum_h_defined
#define __rlc_scoper_enum_h_defined

#include "../src/string.h"
#include "../parser/enum.h"
#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A scoped enum constant. */
struct RlcScopedEnumConstant
{
	RLC_DERIVE(struct, RlcScopedScopeEntry);
	/** The enum constant's numeric value. */
	RlcSrcIndex fValue;
	/** The enum type. */
	struct RlcScopedEnum * fType;
};

/** A scoped enum. */
struct RlcScopedEnum
{
	RLC_DERIVE(struct, RlcScopedScopeEntry);
	/** The amount of unique enum values. */
	RlcSrcSize fSize;
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
struct RlcScopedEnum * rlc_scoped_enum_new(
	struct RlcSrcFile const * file,
	struct RlcParsedEnum const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped enum.
@memberof RlcScopedEnum
@param[in,out] this:
	The enum to destroy.
	@dassert @nonnull */
void rlc_scoped_enum_destroy(
	struct RlcScopedEnum * this);

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
	RlcSrcIndex nameIndex,
	RlcSrcIndex value,
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