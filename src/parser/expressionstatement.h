#ifndef __rlc_parser_expression_statement_h_defined
#define __rlc_parser_expression_statement_h_defined

#include "statement.h"
#include "expression.h"
#include "parser.h"

#include "../macros.h"

/** Expression statement class.
@implements RlcParsedStatement */
struct RlcParsedExpressionStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);

	struct RlcParsedExpression * fExpression;
};

/** Creates an expression statement.
@memberof RlcParsedExpressionStatement
@param[out] this:
	The expression statement to create.
	@dassert @nonnull */
void rlc_parsed_expression_statement_create(
	struct RlcParsedExpressionStatement * this);

/** Destroys an expression statement.
@memberof RlcParsedExpressionStatement
@param[in,out] this:
	The expression statement to destroy.
	@dassert @nonnull */
void rlc_parsed_expression_statement_destroy(
	struct RlcParsedExpressionStatement * this);

/** Parses an expression statement.
@memberof RlcParsedExpressionStatement
@param[out] out:
	The expression statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_expression_statement_parse(
	struct RlcParsedExpressionStatement * out,
	struct RlcParser * parser);

void rlc_parsed_expression_statement_print(
	struct RlcParsedExpressionStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#endif