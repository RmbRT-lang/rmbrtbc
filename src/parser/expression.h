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
	/** RlcParsedSymbolExpression. */
	kRlcParsedSymbolExpression,
	/** RlcParsedSymbolChildExpression. */
	kRlcParsedSymbolChildExpression,
	/** RlcParsedNumberExpression. */
	kRlcParsedNumberExpression,
	/** RlcParsedCharacterExpression. */
	kRlcParsedCharacterExpression,
	/** RlcParsedStringExpression. */
	kRlcParsedStringExpression,
	/** RlcParsedOperatorExpression. */
	kRlcParsedOperatorExpression,
	/** RlcParsedThisExpression. */
	kRlcParsedThisExpression,
	/** RlcParsedCastExpression. */
	kRlcParsedCastExpression,
	/** RlcParsedSizeofExpression. */
	kRlcParsedSizeofExpression,
	/** RlcParsedSymbolConstantExpression. */
	kRlcParsedSymbolConstantExpression,

	RLC_ENUM_END(RlcParsedExpressionType)
};

/** Virtual expression type.
	Initialising functions must set it to the constant corresponding to the deriving type. The order of the enum entries must not be changed, because a function pointer table relies on it. */
struct RlcParsedExpression
{
	RLC_ABSTRACT(RlcParsedExpression);

	/** The expression's first token. */
	struct RlcToken fStart;
	/** The expression's last token.*/
	struct RlcToken fEnd;
};

/** Creates an expression.
@param[out] this:
	The expression to create.
	@dassert @nonnull
@param[in] type:
	The deriving type.
@param[in] first:
	The expression's first token. */
void rlc_parsed_expression_create(
	struct RlcParsedExpression * this,
	enum RlcParsedExpressionType type,
	struct RlcToken first,
	struct RlcToken last);

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
void rlc_parsed_expression_destroy_base(
	struct RlcParsedExpression * this);

/** Parses an expression.
@memberof RlcParsedExpression
@param[in,out] parser:
	The parser data.
@return
	A pointer to the parsed expression. */
struct RlcParsedExpression * rlc_parsed_expression_parse(
	struct RlcParser * parser,
	int flags);

void rlc_parsed_expression_print(
	struct RlcParsedExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

/** A list of expressions. */
struct RlcParsedExpressionList
{
	/** The expression list's values. */
	struct RlcParsedExpression ** fValues;
	/** The expression list's length.*/
	size_t fCount;
};

/** Creates an expression list.
@memberof RlcParsedExpressionList
@param[out] this:
	The expression list to create.
	@dassert @nonnull */
void rlc_parsed_expression_list_create(
	struct RlcParsedExpressionList * this);

/** Destroys an expression list.
@memberof RlcParsedExpressionList
@param[in,out] this:
	The expression list to destroy.
	@dassert @nonnull */
void rlc_parsed_expression_list_destroy(
	struct RlcParsedExpressionList * this);

/** Appends an expression to an expression list.
@memberof RlcParsedExpressionList
@param[in,out] this:
	The expression list to append an expression to.
	@dassert @nonnull
@param[in] expression:
	The expression to append to the list.
	@pass_pointer_ownership
	@dassert @nonnull */
void rlc_parsed_expression_list_append(
	struct RlcParsedExpressionList * this,
	struct RlcParsedExpression * expression);

#ifdef __cplusplus
}
#endif

#endif