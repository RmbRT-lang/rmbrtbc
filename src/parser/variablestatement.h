#ifndef __rlc_parser_variablestatement_h_defined
#define __rlc_parser_variablestatement_h_defined

#include "statement.h"
#include "parser.h"
#include "variable.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Stores a variable declaration statement.
@implements RlcParsedStatement */
struct RlcParsedVariableStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);
	struct RlcParsedVariable fVariable;
};

/** Creates a variable statement.
@memberof RlcParsedVariableStatement
@param[out] this:
	The variable statement to create.
	@dassert @nonnull */
void rlc_parsed_variable_statement_create(
	struct RlcParsedVariableStatement * this,
	size_t start_index);

/** Destroys a variable statement.
@memberof RlcParsedVariableStatement
@param[out] this:
	The variable statement to destroy.
	@dassert @nonnull */
void rlc_parsed_variable_statement_destroy(
	struct RlcParsedVariableStatement * this);

/** Parses a variable declaration statement.
@memberof RlcParsedVariableStatement
@param[out] out:
	The variable declaration statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
int rlc_parsed_variable_statement_parse(
	struct RlcParsedVariableStatement * out,
	struct RlcParser * parser);

#ifdef __cplusplus
}
#endif


#endif