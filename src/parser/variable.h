/** @file variable.h
	Defines the variable struct. */

#ifndef __rlc_parser_variable_h_defined
#define __rlc_parser_variable_h_defined

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Variables as used in the parser. */
struct RlcVariable
{
	/** The variable type. */
	struct RlcTypeName fType;
	/** The variable name. */
	size_t fNameToken;
	/** The initialising expression, or null. */
	struct RlcExpression * fInitExp;
};


#ifdef __cplusplus
}
#endif

#endif