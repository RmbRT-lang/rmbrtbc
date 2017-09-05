#ifndef __rlc_parser_blockstatement_h_defined
#define __rlc_parser_blockstatement_h_defined

#include "statement.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Block statement consisiting of multiple statements.
@implements RlcParsedStatement */
struct RlcParsedBlockStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);

	/** The statements inside the block statement. */
	struct RlcParsedStatementList fList;
};

/** Creates a block statement.
@memberof RlcParsedBlockStatement
@param[out] this:
	The block statement to create.
	@dassert @nonnull */
void rlc_parsed_block_statement_create(
	struct RlcParsedBlockStatement * this);

/** Destroys a block statement.
@memberof RlcParsedBlockStatement
@param[in,out] this:
	The block statement to destroy.
	@dassert @nonnull */
void rlc_parsed_block_statement_destroy(
	struct RlcParsedBlockStatement * this);

/** Parses a block statement.
@memberof RlcParsedBlockStatement
@param[out] out:
	The block statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Nonzero on success. */
int rlc_parsed_block_statement_parse(
	struct RlcParsedBlockStatement * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif