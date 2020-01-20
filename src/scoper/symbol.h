#ifndef __rlc_scoper_symbol_h_defined
#define __rlc_scoper_symbol_h_defined
#pragma once

#include "../src/string.h"
#include "../parser/symbol.h"

#ifdef __cplusplus
extern "C" {
#endif


/** A scoped symbol child's type. */
enum RlcScopedSymbolChildType
{
	kRlcScopedSymbolChildTypeIdentifier,
	kRlcScopedSymbolChildTypeConstructor,
	kRlcScopedSymbolChildTypeDestructor
};

/** A scoped symbol child. */
const struct RlcScopedSymbolChild
{
	enum RlcScopedSymbolChildType type;
	char const * name;
}	kRlcScopedSymbolChildConstructor,
	kRlcScopedSymbolChildDestructor;

/** A scoped symbol. */
struct RlcScopedSymbol
{
	/** Whether the symbol is to be resolved from the global scope. */
	int fIsAbsolute;
	/** The symbol's children. */
	struct RlcScopedSymbolChild * fChildren;
	/** The symbol's child count. */
	RlcSrcSize fChildCount;
};

/** Creates a scoped symbol child from a parsed symbol child.
@memberof RlcScopedSymbolChild
@param[out] this:
	The symbol child to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed symbol child. */
void rlc_scoped_symbol_child_create(
	struct RlcScopedSymbolChild * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSymbolChild const * parsed);

/** Destroys a scoped symbol child.
@memberof RlcScopedSymbolChild
@param[in,out] this:
	The scoped symbol child to destroy.
	@dassert @nonnull */
void rlc_scoped_symbol_child_destroy(
	struct RlcScopedSymbolChild * this);

/** Creates a scoped symbol from a parsed symbol.
@memberof RlcScopedSymbol
@param[out] this:
	The scoped symbol to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed symbol.
	@dassert @nonnull */
void rlc_scoped_symbol_create(
	struct RlcScopedSymbol * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSymbol const * parsed);

/** Destroys a scoped symbol.
@memberof RlcScopedSymbol
@param[in,out] this:
	The scope symbol to destroy.
	@dassert @nonnull */
void rlc_scoped_symbol_destroy(
	struct RlcScopedSymbol * this);

#ifdef __cplusplus
}
#endif

#endif