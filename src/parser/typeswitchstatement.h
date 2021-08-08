/** @file typeswitchstatement.h
	Contains the definition of the type-switch statement used by the parser. */
#ifndef __rlc_parser_typeswitchstatement_h_defined
#define __rlc_parser_typeswitchstatement_h_defined

#include "statement.h"
#include "casestatement.h"
#include "controllabel.h"
#include "variable.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A type-switch statement.
@implements RlcParsedStatement */
struct RlcParsedTypeSwitchStatement
{
	RLC_DERIVE(struct, RlcParsedStatement);

	/** The switch statement's control label. */
	struct RlcParsedControlLabel fLabel;
	/** Whether to switch the actual or derived type. */
	int fIsStatic;
	/** The expression whose type to switch. */
	struct RlcParsedExpression * fExpression;
	/** The switch statement's cases. */
	struct RlcParsedTypeCaseStatement * fCases;
	/** The switch statement's case count. */
	size_t fCaseCount;
};

struct RlcParsedTypeCaseStatement
{
	int fIsDefault;
	struct RlcParsedSymbol fTypeName;
	struct RlcParsedStatement * fBodyStatement;
};

/** Initialises a type-switch statement.
@memberof RlcParsedTypeSwitchStatement
@param[out] this:
	The switch statement to initialise. */
void rlc_parsed_type_switch_statement_create(
	struct RlcParsedTypeSwitchStatement * this);

/** Destroys a type-switch statement.
@memberof RlcParsedTypeSwitchStatement

@param[in,out] this:
	The switch statement to destroy. */
void rlc_parsed_type_switch_statement_destroy(
	struct RlcParsedTypeSwitchStatement * this);

/** Parses a type-switch statement.
@memberof RlcParsedTypeSwitchStatement
@param[out] out:
	The switch statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the switch statement could be parsed. */
_Nodiscard int rlc_parsed_type_switch_statement_parse(
	struct RlcParsedTypeSwitchStatement * out,
	struct RlcParser * parser);

void rlc_parsed_type_switch_statement_print(
	struct RlcParsedTypeSwitchStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif