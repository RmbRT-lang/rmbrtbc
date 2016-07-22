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

	struct RlcParsedFunction ** fFunctions;
	size_t fFunctionCount;

	struct RlcParsedClass ** fClasses;
	size_t fClassCount;

	struct RlcParsedStruct ** fStructs;
	size_t fStructCount;

	struct RlcParsedRawType ** fRawTypes;
	size_t fRawTypeCount;

	struct RlcParsedUnion ** fUnions;
	size_t fUnionCount;

	struct RlcParsedVariable ** fVariables;
	size_t fVariableCount;

	struct RlcParsedEnum ** fEnums;
	size_t fEnumCount;
};

#ifdef __cplusplus
}
#endif

#endif