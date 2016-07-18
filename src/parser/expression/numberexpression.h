/** @file numberexpression.h
	Contains the definition of the numerical expression type. */
#ifndef __rlc_parser_expression_numberexpression_h_defined
#define __rlc_parser_expression_numberexpression_h_defined

#include "expression.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A numerical expression.
	Numerical expressions can either be numbers or single characters.
@implements RlcExpression */
struct RlcNumberExpression
{
	/** Must be initialized to kRlcNumberExpression. */
	enum RlcExpression const fRlcExpression;
	/** The number token's index. */
	size_t fNumberToken;
};

/** Creates a number expression.
@memberof RlcNumberExpression

@param[in] token_index:
	The number token's index. */
struct RlcNumberExpression * rlc_number_expression_create(
	size_t token_index);

/** Destroys a number expression.
@memberof RlcNumberExpression

@param[in] this:
	The number expression to destroy. */
void rlc_number_expression_destroy(
	struct RlcNumberExpression * this);

#ifdef __cplusplus
}
#endif

#endif