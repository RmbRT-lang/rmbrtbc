/** @file symbol.h
	Defines the symbol structure. */

#ifndef __rlc_parser_symbol_h_defined
#define __rlc_parser_symbol_h_defined

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A single identifier. */
struct RlcSymbolChild
{
	/** The index of the token */
	size_t fNameToken;
};

/** A (possibly) namespace-qualified symbol. */
struct RlcSymbol
{
	/** The symbol children. */
	struct RlcSymbolChild * fChildren;
	/** The count of symbol children. */
	size_t fChildCount;
	/** Whether the symbol was prefixed with ```::``` to access the root namespace. */
	int fIsRoot;
};

#ifdef __cplusplus
}
#endif

#endif