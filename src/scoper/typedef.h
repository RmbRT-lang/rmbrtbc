/** @file typedef.h
	Contains the scoped typedef type. */
#ifndef __rlc_scoper_typedef_h_defined
#define __rlc_scoper_typedef_h_defined

#include "scopeentry.h"
#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedTypedef;

/** Scoped typedef type.
@implements RlcScopedScopeEntry */
struct RlcScopedTypedef
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);

	/** The typedef's target type. */
	struct RlcScopedTypeName type;
};

/** Creates a scoped typedef from a parsed typedef.
@memberof RlcScopedTypedef
@param[out] this:
	The scoped typedef to create.
	@dassert @nonnull
@param[in] file:
	The typedef's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed typedef.
	@dassert @nonnull
@param[in] group:
	The typedef's scope item group.
	@dassert @nonnull */
void rlc_scoped_typedef_create(
	struct RlcScopedTypedef * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTypedef const * parsed,
	struct RlcScopedScopeItemGroup * group);

/** Destroys a scoped typedef.
@memberof RlcScopedTypedef
@param[in,out] this:
	The scoped typedef to destroy.
	@dassert @nonnull */
void rlc_scoped_typedef_destroy(
	struct RlcScopedTypedef * this);

#ifdef __cplusplus
}
#endif

#endif