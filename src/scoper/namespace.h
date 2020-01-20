#ifndef __rlc_scoper_namespace_h_defined
#define __rlc_scoper_namespace_h_defined

#include "../parser/namespace.h"
#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A scoped namespace. */
struct RlcScopedNamespace
{
	RLC_DERIVE(struct, RlcScopedScopeEntry);
};

/** Creates a scoped namespace from a parsed namespace.
@memberof RlcScopedNamespace
@param[in] file:
	The parsed namespace's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed namespace.
	@dassert @nonnull
@return
	The created namespace. */
struct RlcScopedNamespace * rlc_scoped_namespace_new(
	struct RlcSrcFile const * file,
	struct RlcParsedNamespace const * parsed,
	struct RlcScopedScope * parent);

/** Destroys a scoped namespace.
@memberof RlcScopedNamespace
@param[in,out] this:
	The namespace to destroy.
	@dassert @nonnull */
void rlc_scoped_namespace_delete(
	struct RlcScopedNamespace * this);

#ifdef __cplusplus
}
#endif


#endif