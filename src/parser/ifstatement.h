/** @file ifstatement.h
	Contains the if/else statement type used by the parser. */
#ifndef __rlc_parser_ifstatement_h_defined
#define __rlc_parser_ifstatement_h_defined

#include "controllabel.h"
#include "expression.h"
#include "statement.h"
#include "variable.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An if/else statement.
@implements RlcParsedStatement */
struct RlcParsedIfStatement
{
	RLC_DERIVE(struct, RlcParsedStatement);

	/** The if statement's condition. */
	struct {
		union {
			/** A condition expression. */
			struct RlcParsedExpression * fExpression;
			/** A variable initialised with a condition expression. */
			struct RlcParsedVariable fVariable;
		};
		/** Whether the condition is a variable or an expression. */
		int fIsVariable;
	} fCondition;

	/** The if label. */
	struct RlcControlLabel fIfLabel;
	/** The else label. */
	struct RlcControlLabel fElseLabel;

	/** Statement to be executed if the condition evaluates to true. */
	struct RlcParsedStatement * fIf;
	/** Optional statement to be executed if the condition evaluates to false.*/
	struct RlcParsedStatement * fElse;
};

/** Creates an if statement.
@memberof RlcParsedIfStatement
@param[out] this:
	The if statement to create.
	@dassert @nonnull */
void rlc_parsed_if_statement_create(
	struct RlcParsedIfStatement * this);

/** Destroys an if statement.
@memberof RlcParsedIfStatement
@param[out] this:
	The if statement to destroy.
	@dassert @nonnull */
void rlc_parsed_if_statement_destroy(
	struct RlcParsedIfStatement * this);

/** Parses an if statement.
@memberof RlcParsedIfStatement
@param[out] out:
	The if statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
int rlc_parsed_if_statement_parse(
	struct RlcParsedIfStatement * out,
	struct RlcParser * parser);

#ifdef __cplusplus
}
#endif


#endif