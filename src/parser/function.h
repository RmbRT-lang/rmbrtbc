/** @file function.h
	Defines the function structure used by the parser. */
#ifndef __rlc_parser_function_h_defined
#define __rlc_parser_function_h_defined

#include <stddef.h>
#include "typename.h"
#include "variable.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A function as used in the parser. */
struct RlcParsedFunction
{
	/** The token index of the first appearance. */
	size_t fDeclarationIndex;
	/** The name of the function.*/
	size_t fNameToken;

	/** The return type of the function. */
	struct RlcParsedTypeName fReturnType;

	/** The arguments the function takes. */
	struct RlcParsedVariable * fArguments;

	/** Whether the function is inline. */
	int fIsInline;
};

#ifdef __cplusplus
}
#endif

#endif