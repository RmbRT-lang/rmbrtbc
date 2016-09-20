/** @file namespace.h
	Defines the namespace structure. */
#ifndef __rlc_parser_namespace_h_defined
#define __rlc_parser_namespace_h_defined

#include <stddef.h>

#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Namespace structure used by the parser.
@implements RlcParsedScopeEntry */
struct RlcParsedNamespace
{
	/** The scope entry base class. */
	struct RlcParsedScopeEntry fScopeEntry;

	/** The scope entries of the namespace. */
	struct RlcParsedScopeEntry ** fEntries;
	/** The scope entry count. */
	size_t fEntryCount;
};

/** Destroys a namespace
@memberof RlcParsedNamespace

@param[in] this:
	The namespace to destroy. */
void rlc_parsed_namespace_destroy(
	struct RlcParsedNamespace * this);

/** Finds an entry with the */
struct RlcParsedScopeEntry * rlc_parsed_namespace_find_entry(
	size_t entryNameToken);

#ifdef __cplusplus
}
#endif

#endif