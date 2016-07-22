/** @file expression.h
	Defines the various expression types. */
#ifndef __rlc_parser_expression_h_defined
#define __rlc_parser_expression_h_defined

#include <stddef.h>
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Virtual expression type.
	Deriving types must have their first field of this type and initialiting functions must set it to the constant corresponding to the deriving type. The order of the enum entries must not be changed, because a function pointer table relies on it.
@interface RlcParsedExpression */
enum RlcParsedExpression {
	/** RlcParsedSymbolExpression */
	kRlcParsedSymbolExpression,
	/** RlcParsedNumberExpression */
	kRlcParsedNumberExpression,

	RLC_ENUM_END(RlcParsedExpression)
};

/** Destroys an expression.
@pure @memberof RlcParsedExpression

@param[in] this:
	The expression to destroy.*/
void rlc_parsed_expression_destroy(
	enum RlcParsedExpression * this);

#ifdef __cplusplus
}
#endif

#endif