#ifndef __rlc_parser_expression_typeexpression_h_defined
#define __rlc_parser_expression_typeexpression_h_defined

#include "expression.h"
#include "../typename.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A type name expression. */
struct RlcParsedTypeExpression
{
	RLC_DERIVE(struct, RlcParsedExpression);
	/** The type name. */
	struct RlcParsedTypeName fType;
};


#ifdef __cplusplus
}
#endif


#endif