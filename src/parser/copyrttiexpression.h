/** @file copyrttiexpression.h
	Contains the COPY_RTTI expression type as used by the parser. */
#ifndef __rlc_parser_copy_rttiexpression_h_defined
#define __rlc_parser_copy_rttiexpression_h_defined

#include "expression.h"
#include "../macros.h"


#ifdef __cplusplus
extern "C" {
#endif

/** A copy_rtti expression as used by the parser. */
struct RlcParsedCopyRttiExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	struct RlcParsedExpression * fSource;
	struct RlcParsedExpression * fDest;
};

/** Creates a copy_rtti expression.
@memberof RlcParsedCopyRttiExpression
@param[out] this:
	The copy_rtti expression to create.
	@dassert @nonull
@param[in] start:
	The expression's first token. */
void rlc_parsed_copy_rtti_expression_create(
	struct RlcParsedCopyRttiExpression * this,
	struct RlcToken start,
	struct RlcToken end);

/** Destroys a copy_rtti expression.
@memberof RlcParsedCopyRttiExpression
@param[in,out] this:
	The copy_rtti expression to destroy.
	@dassert @nonull */
void rlc_parsed_copy_rtti_expression_destroy(
	struct RlcParsedCopyRttiExpression * this);

/** Parses a copy_rtti expression.
@memberof RlcParsedCopyRttiExpression
@param[out] out:
	The copy_rtti expression to parse.
	@dassert @nonnull
@param[in,out] paraser:
	The parser data.
	@dassert @nonnull
@return
	Whether the expression was successfully parsed. */
_Nodiscard int rlc_parsed_copy_rtti_expression_parse(
	struct RlcParsedCopyRttiExpression * out,
	struct RlcParser * parser);

void rlc_parsed_copy_rtti_expression_print(
	struct RlcParsedCopyRttiExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif


#endif