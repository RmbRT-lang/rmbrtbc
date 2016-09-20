/** @file expression.h
	Defines the various expression types. */
#ifndef __rlc_parser_expression_h_defined
#define __rlc_parser_expression_h_defined

#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/** Deriving types of RlcParsedExpression. */
enum RlcParsedExpressionType {
	/** RlcParsedSymbolExpression */
	kRlcParsedSymbolExpression,
	/** RlcParsedNumberExpression */
	kRlcParsedNumberExpression,

	RLC_ENUM_END(RlcParsedExpression)
};

/** Virtual expression type.
	Deriving types must have their first field of this type and initialiting functions must set it to the constant corresponding to the deriving type. The order of the enum entries must not be changed, because a function pointer table relies on it. */
struct RlcParsedExpression
{
	/** The deriving type. */
	enum RlcParsedExpressionType fType;

	/** The expression's first token. */
	size_t fFirst;
	/** The expression's last token.*/
	size_t fLast;
};

/** Destroys an expression.
@memberof RlcParsedExpression
@param[in] this:
	The expression to destroy.*/
void rlc_parsed_expression_destroy(
	struct RlcParsedExpression * this);

#ifdef __cplusplus
}
#endif

#endif