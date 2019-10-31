/** @file switchstatement.h
	Contains the definition of the switch statement used by the parser. */
#ifndef __rlc_parser_switchstatement_h_defined
#define __rlc_parser_switchstatement_h_defined

#include "statement.h"
#include "casestatement.h"
#include "controllabel.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A switch statement.
@implements RlcParsedStatement */
struct RlcParsedSwitchStatement
{
	RLC_DERIVE(struct, RlcParsedStatement);

	/** The switch statement's control label. */
	struct RlcControlLabel fLabel;
	/** The value to be switched. */
	struct RlcParsedExpression * fSwitchValue;
	/** The switch statement's cases. */
	struct RlcParsedCaseStatement * fCases;
	/** The switch statement's case count. */
	size_t fCaseCount;
};

/** Initialises a switch statement.
@memberof RlcParsedSwitchStatement
@param[out] this:
	The switch statement to initialise. */
void rlc_parsed_switch_statement_create(
	struct RlcParsedSwitchStatement * this);

/** Destroys a switch statement.
@memberof RlcParsedSwitchStatement

@param[in,out] this:
	The switch statement to destroy. */
void rlc_parsed_switch_statement_destroy(
	struct RlcParsedSwitchStatement * this);

/** Parses a switch statement.
@memberof RlcParsedSwitchStatement
@param[out] out:
	The switch statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the switch statement could be parsed. */
int rlc_parsed_switch_statement_parse(
	struct RlcParsedSwitchStatement * out,
	struct RlcParser * parser);

/** Adds a case to a switch statement.
@memberof RlcParsedSwitchStatement
@param[in,out] this:
	The switch statement to add a case to.
@param[in] case_stmt:
	The case to add. */
void rlc_parsed_switch_statement_add_case(
	struct RlcParsedSwitchStatement * this,
	struct RlcParsedCaseStatement * case_stmt);

#ifdef __cplusplus
}
#endif

#endif