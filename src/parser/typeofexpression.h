/** @file typeofexpression.h
	Contains the typeof expression type as used by the parser. */
#ifndef __rlc_parser_typeofexpression_h_defined
#define __rlc_parser_typeofexpression_h_defined

#include "expression.h"
#include "typename.h"
#include "../macros.h"


#ifdef __cplusplus
extern "C" {
#endif

/** A typeof expression as used by the parser. */
struct RlcParsedTypeofExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	int fIsTypeArg;
	int fIsStatic;
	union {
		struct RlcParsedTypeName fType;
		struct RlcParsedExpression * fExpression;
	};
};

/** Creates a typeof expression.
@memberof RlcParsedTypeofExpression
@param[out] this:
	The typeof expression to create.
	@dassert @nonull
@param[in] start:
	The expression's first token. */
void rlc_parsed_typeof_expression_create(
	struct RlcParsedTypeofExpression * this,
	struct RlcToken start,
	struct RlcToken end);

/** Destroys a typeof expression.
@memberof RlcParsedTypeofExpression
@param[in,out] this:
	The typeof expression to destroy.
	@dassert @nonull */
void rlc_parsed_typeof_expression_destroy(
	struct RlcParsedTypeofExpression * this);

/** Parses a typeof expression.
@memberof RlcParsedTypeofExpression
@param[out] out:
	The typeof expression to parse.
	@dassert @nonnull
@param[in,out] paraser:
	The parser data.
	@dassert @nonnull
@return
	Whether the expression was successfully parsed. */
_Nodiscard int rlc_parsed_typeof_expression_parse(
	struct RlcParsedTypeofExpression * out,
	struct RlcParser * parser);

void rlc_parsed_typeof_expression_print(
	struct RlcParsedTypeofExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif


#endif