/** @file expression.h
	Defines the various expression types. */
#ifndef __rlc_parser_expression_h_defined
#define __rlc_parser_expression_h_defined

#include <stddef.h>
#include "../../macros.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Virtual expression type.
	Deriving types must have their first field of this type and initialiting functions must set it to the constant corresponding to the deriving type. The order of the enum entries must not be changed, because a function pointer table relies on it.
@interface RlcExpression */
enum RlcExpression {
	/** RlcSymbolExpression */
	kRlcSymbolExpression,
	/** RlcNumberExpression */
	kRlcNumberExpression,

	RLC_ENUM_END(RlcExpression)
};

/** Destroys an expression.
@pure @memberof RlcExpression

@param[in] this:
	The expression to destroy.*/
void rlc_expression_destroy(
	enum RlcExpression * this);

#ifdef __cplusplus
}
#endif

#endif