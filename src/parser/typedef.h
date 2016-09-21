/** @file typedef.h
	Contains the type for type aliasing used by the parser. */
#ifndef __rlc_parser_typedef_h_defined
#define __rlc_parser_typedef_h_defined

#include "parser.h"
#include "typename.h"
#include "scopeentry.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Describes a typedef as used by the parser.
@implements RlcParsedScopeEntry */
struct RlcParsedTypedef
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The type name that has been aliased. */
	struct RlcParsedTypeName fType;
};

/** Destroys a typedef.
@param[out] this:
	The typedef to destroy. */
void rlc_parsed_typedef_destroy(
	struct RlcParsedTypedef * this);

#ifdef __cplusplus
}
#endif

#endif