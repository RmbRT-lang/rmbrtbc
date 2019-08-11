/** @file breakstatement.h
	Contains the break statement used by the parser. */
#ifndef __rlc_parser_breakstatement_h_defined
#define __rlc_parser_breakstatement_h_defined

#include "statement.h"
#include "controllabel.h"
#include "parser.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A break statement as used by the parser.
@imlements RlcParsedStatement */
struct RlcParsedBreakStatement
{
	RLC_DERIVE(struct, RlcParsedStatement);

	/** The break statement's label. */
	struct RlcControlLabel fLabel;
};

/** Creates a break statement.
@memberof RlcParsedBreakStatement
@param[out] this:
	The break statement to create.
	@dassert @nonnull */
void rlc_parsed_break_statement_create(
	struct RlcParsedBreakStatement * this);

/** Destroys a break statement.
@memberof RlcParsedBreakStatement
@param[in,out] this:
	The break statement to destroy.
	@dassert @nonnull */
void rlc_parsed_break_statement_destroy(
	struct RlcParsedBreakStatement * this);

/** Parses a break statement.
@memberof RlcParsedBreakStatement
@param[out] out:
	The break statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the break statement was successfully parsed. */
int rlc_parsed_break_statement_parse(
	struct RlcParsedBreakStatement * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif