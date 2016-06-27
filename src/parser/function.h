/** @file function.h
	Defines the function structure. */

#ifndef __rlc_parser_function_h_defined
#define __rlc_parser_function_h_defined

#include <stddef.h>
#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A function */
struct RlcFunction
{
	/** The token index of the first appearance. */
	size_t fDeclarationIndex;
	/** The name of the function.*/
	size_t fNameToken;

	/** The return type of the function. */
	struct RlcTypeName fReturnType;
	/** Whether the function is inline. */
	int fIsInline;
};

#ifdef __cplusplus
}
#endif

#endif