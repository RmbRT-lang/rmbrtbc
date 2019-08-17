/** @file externsymbol.h
	Contains the external symbol declaration type used by the parser. */
#ifndef __rlc_parser_externalsymbol_h_defined
#define __rlc_parser_externalsymbol_h_defined

#include "scopeentry.h"
#include "typename.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** External symbol declaration.
@extends RlcParsedScopeEntry */
struct RlcParsedExternalSymbol
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** Whether the external symbol has a custom link name (i.e., it is not from RL). */
	int fHasCustomLinkName;
	/** The symbol's custom link name. */
	size_t fCustomLinkName;
	/** The symbol's type. */
	struct RlcParsedTypeName fType;
};

/** Creates an external symbol.
@memberof RlcParsedExternalSymbol
@param[out] this:
	The external symbol to create.
	@dassert @nonnull
@param[in] start:
	The start index of the token stream. */
void rlc_parsed_external_symbol_create(
	struct RlcParsedExternalSymbol * this,
	size_t start);

/** Destroys an external symbol.
@memberof RlcParsedExternalSymbol
@param[in,out] this:
	The external symbol to destroy.
	@dassert @nonnull */
void rlc_parsed_external_symbol_destroy(
	struct RlcParsedExternalSymbol * this);

/** Parses an external symbol declaration.
@memberof RlcParsedExternalSymbol
@param[out] out:
	The external symbol to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the external symbol was successfully parsed. */
int rlc_parsed_external_symbol_parse(
	struct RlcParsedExternalSymbol * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif