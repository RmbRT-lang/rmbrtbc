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
	RLC_DERIVE(struct, RlcParsedExpression);

	/** The expression's symbol. */
	struct RlcParsedSymbol fSymbol;
};

/** Initialises a symbol expression.
@memberof RlcParsedSymbolExpression
@param[in, out] this:
	The symbol expression to create. */
void rlc_parsed_symbol_expression_create(
	struct RlcParsedSymbolExpression * this);

/** Destroys a symbol expression.
@memberof RlcParsedSymbolExpression

@param[in] this:
	The symbol exression to destroy. */
void rlc_parsed_symbol_expression_destroy(
	struct RlcParsedSymbolExpression * this);

/** Parses a symbol expression.
@memberof RlcParsedSymbolExpression
@param[out] out:
	The symbol expression to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnul
@return
	Nonzero on success. */
int rlc_parsed_symbol_expression_parse(
	struct RlcParsedSymbolExpression * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif


#endif