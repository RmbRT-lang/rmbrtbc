/** @file variable.h
	Defines the variable struct. */

#ifndef __rlc_parser_variable_h_defined
#define __rlc_parser_variable_h_defined

#include <stddef.h>

#include "expression.h"
#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Variables as used in the parser. */
struct RlcVariable
{
	/** The variable type. */
	struct RlcParsedTypeName fType;
	/** The variable name. */
	size_t fNameToken;
	/** The initialising expression, or null. */
	enum RlcParsedExpression * fInitExp;
};

void rlc_variable_destroy(
	struct RlcVariable * this);

#ifdef __cplusplus
}
#endif

#endif