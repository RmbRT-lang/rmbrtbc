/** @file statement.h
	Contains the statement base class used by the parser. */
#ifndef __rlc_parser_statement_h_defined
#define __rlc_parser_statement_h_defined

#include "parser.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Deriving statement types. */
enum RlcParsedStatementType
{
	kRlcParsedExpressionStatement,
	kRlcParsedBlockStatement,
	kRlcParsedIfStatement,
	kRlcParsedLoopStatement,
	kRlcParsedVariableStatement,
	kRlcParsedReturnStatement,
	kRlcParsedSwitchStatement,
	kRlcParsedCaseStatement,
	kRlcParsedBreakStatement,
	kRlcParsedTryStatement,
	kRlcParsedThrowStatement,
	RLC_ENUM_END(RlcParsedStatementType)
};

/** Statement base class used by the parser. */
struct RlcParsedStatement
{
	RLC_ABSTRACT(RlcParsedStatement);
};

/** Creates a parsed statement.
@memberof RlcParsedStatement
@param[out] this:
	The statement to create.
	@dassert @nonnull
@param[in] type:
	The deriving type. */
void rlc_parsed_statement_create(
	struct RlcParsedStatement * this,
	enum RlcParsedStatementType type);

void rlc_parsed_statement_destroy_base(
	struct RlcParsedStatement * this);

/** Destroys a parsed statement.
@virtual @memberof RlcParsedStatement
@param[in,out] this:
	The statement to destroy.
	@dassert @nonnull */
void rlc_parsed_statement_destroy_virtual(
	struct RlcParsedStatement * this);

/** Parses a statement.
@memberof RlcParsedStatement
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[in] flags:
	The statements that are allowed. 
@return
	`NULL`, if no statement could be parsed, otherwise a pointer to a dynamically allocated instance of the deriving type. */
struct RlcParsedStatement * rlc_parsed_statement_parse(
	struct RlcParser * parser,
	int flags);

/** Contains a statement list. */
struct RlcParsedStatementList
{
	/** The statement pointers. */
	struct RlcParsedStatement ** fStatements;
	/** The element count. */
	size_t fStatementCount;
};

/** Creates a statement list.
@memberof RlcParsedStatementList
@param[out] this:
	The statement list to create.
	@dassert @nonnull */
void rlc_parsed_statement_list_create(
	struct RlcParsedStatementList * this);
/** Destroys a statement list.
@memberof RlcParsedStatementList
@param[in,out] this:
	The statement list to destroy.
	@dassert @nonnull */
void rlc_parsed_statement_list_destroy(
	struct RlcParsedStatementList * this);

/** Adds a statement to a statement list.
@memberof RlcParsedStatementList
@param[in,out] this:
	The statement list to add to.
	@dassert @nonnull
@param[in] stmt:
	The statement to add to the list.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_statement_list_add(
	struct RlcParsedStatementList * this,
	struct RlcParsedStatement * stmt);

#ifdef __cplusplus
}
#endif



#endif