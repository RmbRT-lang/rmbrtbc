/** @file externalsymbol.h
	Contains the scoped external symbol type. */
#ifndef __rlc_scoper_externalsymbol_h_defined
#define __rlc_scoper_externalsymbol_h_defined

#include "scopeentry.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcSrcFile;
struct RlcParsedExternalSymbol;

#define kRlcScopedExternalSymbol kRlcParsedExternalSymbol

struct RlcScopedExternalSymbol
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);

	/** Whether the external symbol has a custom link name. */
	char hasCustomLinkName;
	/** The external symbol's custom link name, if any. */
	struct RlcScopedText customLinkName;
	/** The external symbol's type. */
	struct RlcScopedTypeName type;
};

/** Creates a scoped external symbol from a parsed external symbol.
@memberof RlcScopedExternalSymbol
@param[out] this:
	The scoped external symbol to create.
	@dassert
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed external symbol.
	@dassert @nonnull
@param[in,out] parent:
	The scope item group containing the external symbol.
	@dassert @nonnull */
void rlc_scoped_external_symbol_create(
	struct RlcScopedExternalSymbol * this,
	struct RlcSrcFile const * file,
	struct RlcParsedExternalSymbol const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped external symbol.
@memberof RlcScopedExternalSymbol
@param[in,out] this:
	The scoped external symbol to destroy.
	@dassert @nonnull */
void rlc_scoped_external_symbol_destroy(
	struct RlcScopedExternalSymbol * this);

#ifdef __cplusplus
}
#endif


#endif