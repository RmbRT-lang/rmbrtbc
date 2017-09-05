/** @file namespace.h
	Defines the namespace structure. */
#ifndef __rlc_parser_namespace_h_defined
#define __rlc_parser_namespace_h_defined

#include "scopeentry.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Namespace structure used by the parser.
@implements RlcParsedScopeEntry */
struct RlcParsedNamespace
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);
	/** The scope entry list. */
	struct RlcParsedScopeEntryList fEntryList;
};

/** Creates a namespace.
@memberof RlcParsedNamespace

@param[out] this:
	The namespace to create.
	@dassert @nonnull */
void rlc_parsed_namespace_create(
	struct RlcParsedNamespace * this,
	size_t start_index);

/** Destroys a namespace.
@memberof RlcParsedNamespace

@param[in,out] this:
	The namespace to destroy.
	@dassert @nonnull */
void rlc_parsed_namespace_destroy(
	struct RlcParsedNamespace * this);

/** Parses a namespace.
@param[out] out:
	The namespace to parse.
@param[in,out] parser:
	The parser data.
@return
	Whether the parsing succeeded. */
int rlc_parsed_namespace_parse(
	struct RlcParsedNamespace * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif