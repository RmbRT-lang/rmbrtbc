/** @file
	Contains the loop statement class. */
#ifndef __rlc_parser_loopstatement_h_defined
#define __rlc_parser_loopstatement_h_defined

#include "controllabel.h"
#include "statement.h"
#include "parser.h"
#include "variable.h"
#include "expression.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Stores any kind of loop statement.
@implements RlcParsedStatement */
struct RlcParsedLoopStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);

	/** The loop's control label. */
	struct RlcParsedControlLabel fLabel;
	/** Whether the condition is checked before or after the loop body. */
	int fIsPostCondition;
	/** Whether the condition is a variable declaration or an expression. */
	int fIsVariableCondition;
	/** Whether the initial statement is a variable declaration or an expression. */
	int fIsVariableInitial;
	/** Whether the loop is a range loop with an iterator. */
	int fIsRangeLoop;
	int fIsReverseRangeLoop;
	/** The initial loop statement, if any. */
	union {
		/** The loop statement's initial expression. */
		struct RlcParsedExpression * fExpression;
		/** The loop statement's initial variable declaration.*/
		struct RlcParsedVariable fVariable;
	} fInitial;
	/** The loop statement's loop condition. */
	union {
		/** The loop statement's condition expression. */
		struct RlcParsedExpression * fExpression;
		/** A variable storing the condition expression's result. */
		struct RlcParsedVariable fVariable;
	} fCondition;
	/** The loop statement's body. */
	struct RlcParsedStatement * fBody;
	/** The loop statement's post iteration step. */
	struct RlcParsedExpression * fPostLoop;
};

/** Creates a loop statement.
@memberof RlcParsedLoopStatement
@param[out] this:
	The loop statement to create.
	@dassert @nonnull */
void rlc_parsed_loop_statement_create(
	struct RlcParsedLoopStatement * this);
/** Destroys a loop statement.
@memberof RlcParsedLoopStatement
@param[out] this:
	The loop statement to destroy.
	@dassert @nonnull */
void rlc_parsed_loop_statement_destroy(
	struct RlcParsedLoopStatement * this);

/** Parses a loop statement.
@memberof RlcParsedLoopStatement
@param[out] out:
	The loop statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull */
int rlc_parsed_loop_statement_parse(
	struct RlcParsedLoopStatement * out,
	struct RlcParser * parser);

void rlc_parsed_loop_statement_print(
	struct RlcParsedLoopStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif


#endif