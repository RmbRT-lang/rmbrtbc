/** @file sizeofexpression.h
	Contains the sizeof expression type as used by the parser. */
#ifndef __rlc_parser_sizeofexpression_h_defined
#define __rlc_parser_sizeofexpression_h_defined

#include "expression.h"
#include "typename.h"
#include "../macros.h"


#ifdef __cplusplus
extern "C" {
#endif

/** A sizeof expression as used by the parser. */
struct RlcParsedSizeofExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	int fIsType;
	union {
		struct RlcParsedTypeName fType;
		struct RlcParsedExpression * fExpression;
	};
};

/** Creates a sizeof expression.
@memberof RlcParsedSizeofExpression
@param[out] this:
	The sizeof expression to create.
	@dassert @nonull
@param[in] start:
	The expression's first token. */
void rlc_parsed_sizeof_expression_create(
	struct RlcParsedSizeofExpression * this,
	struct RlcToken start,
	struct RlcToken end);

/** Destroys a sizeof expression.
@memberof RlcParsedSizeofExpression
@param[in,out] this:
	The sizeof expression to destroy.
	@dassert @nonull */
void rlc_parsed_sizeof_expression_destroy(
	struct RlcParsedSizeofExpression * this);

/** Parses a sizeof expression.
@memberof RlcParsedSizeofExpression
@param[out] out:
	The sizeof expression to parse.
	@dassert @nonnull
@param[in,out] paraser:
	The parser data.
	@dassert @nonnull
@return
	Whether the expression was successfully parsed. */
_Nodiscard int rlc_parsed_sizeof_expression_parse(
	struct RlcParsedSizeofExpression * out,
	struct RlcParser * parser);

void rlc_parsed_sizeof_expression_print(
	struct RlcParsedSizeofExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif


#endif