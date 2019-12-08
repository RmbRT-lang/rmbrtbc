#ifndef __rlc_parser_throwstatement_h_defined
#define __rlc_parser_throwstatement_h_defined

#include "statement.h"
#include "expression.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

enum RlcThrowType
{
	/** Rethrow statements. */
	kRlcThrowTypeRethrow,
	/** Throw statements with a value. */
	kRlcThrowTypeValue,
	/** Value-less throw statements. */
	kRlcThrowTypeVoid
};

/** A throw statement as used by the parser. */
struct RlcParsedThrowStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);

	/** The throw type. */
	enum RlcThrowType fType;
	/** The thrown value.
		Only exists if it is a value throw statement. */
	struct RlcParsedExpression * fValue;
};

/** Creates a parsed throw statement.
@memberof RlcParsedThrowStatement
@param[out] this:
	The throw statement to create.
	@dassert @nonnull */
void rlc_parsed_throw_statement_create(
	struct RlcParsedThrowStatement * this);

/** Destroys a parsed throw statement.
@memberof RlcParsedThrowStatement
@param[in,out] this:
	The throw statement to destroy.
	@dassert @nonnull */
void rlc_parsed_throw_statement_destroy(
	struct RlcParsedThrowStatement * this);

/** Parses a parsed throw statement.
@memberof RlcParsedThrowStatement
@param[out] out:
	The throw statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser.
	@dassert @nonnull */
_Nodiscard int rlc_parsed_throw_statement_parse(
	struct RlcParsedThrowStatement * out,
	struct RlcParser * parser);

#ifdef __cplusplus
}
#endif

#endif