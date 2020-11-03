/** @file casestatement.h
	Contains the definition of the case statement used by the parser. */
#ifndef __rlc_parser_casestatement_h_defined
#define __rlc_parser_casestatement_h_defined

#include "controllabel.h"
#include "expression.h"
#include "statement.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedSwitchStatement;

/** A switch case statement.
@implements RlcParsedStatement */
struct RlcParsedCaseStatement
{
	RLC_DERIVE(struct, RlcParsedStatement);

	/** Whether it is the default case. */
	int fIsDefault;
	/** The case's control label. */
	struct RlcParsedControlLabel fControlLabel;
	/** The case's values.
		Only exists if it is not the default case. */
	struct RlcParsedExpressionList fValues;

	/** The case's body statement. */
	struct RlcParsedStatement * fBody;
};

/** Initialises a parsed case statement.
@memberof RlcParsedCaseStatement
@param[out] this:
	The case statement to initialise.
	@dassert @nonnull */
void rlc_parsed_case_statement_create(
	struct RlcParsedCaseStatement * this);

/** Destroys a parsed case statement.
@memberof RlcParsedCaseStatement
@param[in,out] this:
	The case statement to destroy.
	@dassert @nonnull */
void rlc_parsed_case_statement_destroy(
	struct RlcParsedCaseStatement * this);

/** Parses a case statement.
@memberof RlcParsedCaseStatement
@param[out] out:
	The case statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the case statement was successfully parsed. */
_Nodiscard int rlc_parsed_case_statement_parse(
	struct RlcParsedCaseStatement * out,
	struct RlcParser * parser);

void rlc_parsed_case_statement_print(
	struct RlcParsedCaseStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out,
	size_t i,
	struct RlcParsedSwitchStatement const * parent);

#ifdef __cplusplus
}
#endif

#endif