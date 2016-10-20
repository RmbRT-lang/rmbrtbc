/** @file expression.h
	Defines the various expression types. */
#ifndef __rlc_parser_expression_h_defined
#define __rlc_parser_expression_h_defined

#include "parser.h"

#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/** Deriving types of RlcParsedExpression. */
enum RlcParsedExpressionType {
	/** RlcParsedSymbolExpression */
	kRlcParsedSymbolExpression,
	/** RlcParsedNumberExpression */
	kRlcParsedNumberExpression,
	/** RlcParsedTypeNameExpression */
	kRlcParsedTypeNameExpression,

	RLC_ENUM_END(RlcParsedExpressionType)
};

/** Virtual expression type.
	Initialising functions must set it to the constant corresponding to the deriving type. The order of the enum entries must not be changed, because a function pointer table relies on it. */
struct RlcParsedExpression
{
	RLC_ABSTRACT(RlcParsedExpression);

	/** The expression's first token. */
	size_t fFirst;
	/** The expression's last token.*/
	size_t fLast;
};

/** Creates an expression.
@param[out] this:
	The expression to create.
	@dassert @nonnull
@param[in] type:
	The deriving type. */
void rlc_parsed_expression_create(
	struct RlcParsedExpression * this,
	enum RlcParsedExpressionType type);

/** Destroys an expression.
@memberof RlcParsedExpression
@param[in] this:
	The expression to destroy.*/
void rlc_parsed_expression_destroy_virtual(
	struct RlcParsedExpression * this);

/** Destroys the expression base instance.
@memberof RlcParsedExpression
@param[in,out] this:
	The expression base instance to destroy.
	@dassert @nonnull */
inline void rlc_parsed_expression_destroy_base(
	struct RlcParsedExpression * this) {}

/** Parses an expression.
@memberof RlcParsedExpression
@param[out] out:
	A pointer to store the address of the parsed expression.
@param[in,out] parser:
	The parser data. */
int rlc_parsed_expression_parse(
	struct RlcParsedExpression ** out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif

#endif