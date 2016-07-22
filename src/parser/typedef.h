/** @file typedef.h
	Contains the type for type aliasing used by the parser. */
#ifndef __rlc_parser_typedef_h_defined
#define __rlc_parser_typedef_h_defined

#include "typename.h"
#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Describes a typedef as used by the parser.
@implements RlcParsedScopeEntry */
struct RlcParsedTypedef
{
	/** The alias of the type. */
	struct RlcParsedScopeEntry fScopeEntry;
	/** The type name that has been aliased. */
	struct RlcParsedTypeName fType;
};

#ifdef __cplusplus
}
#endif

#endif