/** @file continuestatement.h
	Contains the continue statement used by the parser. */
#ifndef __rlc_parser_continuestatement_h_defined
#define __rlc_parser_continuestatement_h_defined

#include "statement.h"
#include "controllabel.h"
#include "parser.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A continue statement as used by the parser.
@imlements RlcParsedStatement */
struct RlcParsedContinueStatement
{
	RLC_DERIVE(struct, RlcParsedStatement);

	/** The continue statement's label. */
	struct RlcParsedControlLabel fLabel;
};

/** Creates a continue statement.
@memberof RlcParsedContinueStatement
@param[out] this:
	The continue statement to create.
	@dassert @nonnull */
void rlc_parsed_continue_statement_create(
	struct RlcParsedContinueStatement * this);

/** Destroys a continue statement.
@memberof RlcParsedContinueStatement
@param[in,out] this:
	The continue statement to destroy.
	@dassert @nonnull */
void rlc_parsed_continue_statement_destroy(
	struct RlcParsedContinueStatement * this);

/** Parses a continue statement.
@memberof RlcParsedContinueStatement
@param[out] out:
	The continue statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the continue statement was successfully parsed. */
int rlc_parsed_continue_statement_parse(
	struct RlcParsedContinueStatement * out,
	struct RlcParser * parser);

void rlc_parsed_continue_statement_print(
	struct RlcParsedContinueStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif