#ifndef __rlc_parser_symbolchildexpression_h_defined
#define __rlc_parser_symbolchildexpression_h_defined

#include "parser.h"
#include "../macros.h"

#include "expression.h"
#include "symbol.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A symbol child expression as used by the parser.
@implements RlcParsedExpression
@implements RlcParsedSymbolChild */
struct RlcParsedSymbolChildExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);
	RLC_DERIVE(struct,RlcParsedSymbolChild);
};

/** Creates a symbol child expression.
@memberof RlcParsedSymbolChildExpression
@param[out] this:
	The symbol child expression to create.
	@dassert @nonnull
@param[in] first:
	The expression's first token. */
void rlc_parsed_symbol_child_expression_create(
	struct RlcParsedSymbolChildExpression * this,
	RlcSrcIndex first);
/** Destroys a symbol child expression.
@memberof RlcParsedSymbolChildExpression
@param[in,out] this:
	The symbol child expression to destroy.
	@dassert @nonnull */
void rlc_parsed_symbol_child_expression_destroy(
	struct RlcParsedSymbolChildExpression * this);
/** Parses a symbol child expression.
@memberof RlcParsedSymbolChildExpression
@param[out] out:
	The symbol child expression to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_symbol_child_expression_parse(
	struct RlcParsedSymbolChildExpression * out,
	struct RlcParser * parser);

void rlc_parsed_symbol_child_expression_print(
	struct RlcParsedSymbolChildExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif