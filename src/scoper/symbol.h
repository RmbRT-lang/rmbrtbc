#ifndef __rlc_scoper_symbol_h_defined
#define __rlc_scoper_symbol_h_defined
#pragma once

#include "../src/string.h"
#include "../parser/scopeentry.h"

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


void rlc_scoped_symbol_child_create(
	struct RlcScopedSymbolChild * this,
	struct RlcSrcFile * file,
	struct RlcParsedScopeEntry * parsed);

void rlc_scoped_symbol_child_destroy(
	struct RlcScopedSymbolChild * this);

int rlc_scoped_symbol_child_compare(
	struct RlcScopedSymbolChild const * a,
	struct RlcScopedSymbolChild const * b);

#ifdef __cplusplus
}
#endif

#endif