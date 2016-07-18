#ifndef __rlc_parser_expression_symbolexpression_h_defined
#define __rlc_parser_expression_symbolexpression_h_defined

#include "expression.h"
#include "../symbol.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Symbol expression type.
	A symbol expression is an exression that contains the name of a code entity.
@implements enum RlcExpression */
struct RlcSymbolExpression
{
	/** Must be initialized to kRlcSymbolExpression. */
	enum RlcExpression const fRlcExpression;
	/** The expression's symbol. */
	struct RlcSymbol fSymbol;
};

/** Allocates and initialises a symbol expression.
@memberof RlcSymbolExpression

@param[in] symbol:
	The expression's symbol.
	@pass_ownership*/
struct RlcSymbolExpression * rlc_symbol_expression_create(
	struct RlcSymbol * symbol);

/** Destroys a symbol expression.
@memberof RlcSymbolExpression

@param[in] this:
	The symbol exression to destroy. */
void rlc_symbol_expression_destroy(
	struct RlcSymbolExpression * this);


#ifdef __cplusplus
}
#endif


#endif