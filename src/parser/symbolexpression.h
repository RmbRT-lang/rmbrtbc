#ifndef __rlc_parser_expression_symbolexpression_h_defined
#define __rlc_parser_expression_symbolexpression_h_defined

#include "expression.h"
#include "symbol.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Symbol expression type.
	A symbol expression is an exression that contains the name of a code entity.
@implements RlcParsedExpression */
struct RlcParsedSymbolExpression
{
	/** Must be initialized to kRlcParsedSymbolExpression. */
	enum RlcParsedExpression const fRlcParsedExpression;
	/** The expression's symbol. */
	struct RlcParsedSymbol fSymbol;
};

/** Allocates and initialises a symbol expression.
@memberof RlcParsedSymbolExpression

@param[in] symbol:
	The expression's symbol.
	@pass_ownership*/
struct RlcParsedSymbolExpression * rlc_symbol_expression_create(
	struct RlcParsedSymbol * symbol);

/** Destroys a symbol expression.
@memberof RlcParsedSymbolExpression

@param[in] this:
	The symbol exression to destroy. */
void rlc_parsed_symbol_expression_destroy(
	struct RlcParsedSymbolExpression * this);


#ifdef __cplusplus
}
#endif


#endif