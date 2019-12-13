#ifndef __rlc_parser_trystatement_h_defined
#define __rlc_parser_trystatement_h_defined

#include "statement.h"
#include "variable.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A try-catch-finally statement as used by the parser.
@implements RlcParsedStatement */
struct RlcParsedTryStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);

	/** The try's body. */
	struct RlcParsedStatement * fBody;
	/** The catch clauses. */
	struct RlcParsedCatchStatement * fCatches;
	/** The catch clause count. */
	RlcSrcSize fCatchCount;
	/** The finally clause. */
	struct RlcParsedStatement * fFinally;
};

/** Creates a parsed try statement.
@memberof RlcParsedTryStatement
@param[out] this:
	The try statement to create.
	@dassert @nonnull */
void rlc_parsed_try_statement_create(
	struct RlcParsedTryStatement * this);
/** Destroys a parsed try statement.
@memberof RlcParsedTryStatement
@param[in,out] this:
	The try statement to destry.
	@dassert @nonnull */
void rlc_parsed_try_statement_destroy(
	struct RlcParsedTryStatement * this);

/** Parses a try statement.
@memberof RlcParsedTryStatement
@param[out] out:
	The try statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser.
	@dassert @nonnull */
_Nodiscard int rlc_parsed_try_statement_parse(
	struct RlcParsedTryStatement * out,
	struct RlcParser * parser);

/** A catch-clause as used by the parser. */
struct RlcParsedCatchStatement
{
	/** Whether it is a void catch. */
	int fIsVoid;
	/** The caught exception variable. */
	struct RlcParsedVariable fException;
	/** The catch clause's body. */
	struct RlcParsedStatement * fBody;
};

/** Parses a parsed catch statement.
@memberof RlcParsedCatchStatement
@param[out] out:
	The parsed catch statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser.
	@dassert @nonnull */
_Nodiscard int rlc_parsed_catch_statement_parse(
	struct RlcParsedCatchStatement * out,
	struct RlcParser * parser);
/** Destroys a parsed catch statement.
@memberof RlcParsedCatchStatement
@param[in,out] this:
	The parsed catch statement to destroy.
	@dassert @nonnull */
void rlc_parsed_catch_statement_destroy(
	struct RlcParsedCatchStatement * this);

#ifdef __cplusplus
}
#endif

#endif