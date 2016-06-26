#ifndef __rlc_parser_namespace_h_defined
#define __rlc_parser_namespace_h_defined

#include <rlc/tokens.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RlcNamespace
{
	/** This is the index of the first token of the first appearance. */
	size_t fDeclarationIndex;
	/** The index of the name token. */
	size_t fNameToken;

	struct RlcFunction ** fFunctions;
	size_t fFunctionCount;

	struct RlcClass ** fClasses;
	size_t fClassCount;

	struct RlcStruct ** fStructs;
	size_t fStructCount;

	struct RlcRawType ** fRawTypes;
	size_t fRawTypeCount;

	struct RlcVariable ** fVariables;
	size_t fVariableCount;

	struct RlcEnum ** fEnums;
	size_t fEnumCount;
};

#ifdef __cplusplus
}
#endif

#endif