/** @file operatorexpression.h
	Contains the operator expression type used by the parser. */
#ifndef __rlc_parser_operatorexpression_h_defined
#define __rlc_parser_operatorexpression_h_defined

#include "expression.h"
#include "parser.h"

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

enum RlcOperator
{
	kAdd, kSub, kMul, kDiv, kMod,
	kEquals, kNotEquals, kLess, kLessEquals, kGreater, kGreaterEquals,
	kBitAnd, kBitOr, kBitXor, kBitNot,
	kLogAnd, kLogOr, kLogNot,
	kShiftLeft, kShiftRight, kRotateLeft, kRotateRight,
	kNeg, kPos,
	kSubscript, kCall, kConditional,
	kMemberReference, kMemberPointer,
	kBindReference, kBindPointer,
	kDereference, kAddress,
	kPreIncrement, kPreDecrement,
	kPostIncrement, kPostDecrement,

	kAsync,
	kFullAsync,
	kExpectDynamic,
	kMaybeDynamic,

	kAssign,
	kAddAssign, kSubAssign, kMulAssign, kDivAssign, kModAssign,
	kBitAndAssign, kBitOrAssign, kBitXorAssign,
	kShiftLeftAssign, kShiftRightAssign
};


/** The operator expression type used by the parser.
@implements RlcParsedExpression */
struct RlcParsedOperatorExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	/** The expression's operator. */
	enum RlcOperator fOperator;
	/** The expression list. */
	struct RlcParsedExpression ** fExpressions;
	/** The expression count. */
	size_t fExpressionCount;
};

/** Creates an operator expression.
@memberof RlcParsedOperatorExpression
@param[out] this:
	The operator expression to create.
	@dassert @nonnull
@param[in] first:
	The first token of the expression. */
void rlc_parsed_operator_expression_create(
	struct RlcParsedOperatorExpression * this,
	RlcSrcIndex first,
	RlcSrcIndex last);

/** Destroys an operator expression.
@memberof RlcParsedOperatorExpression
@param[out] this:
	The operator expression to destroy.
	@dassert @nonnull */
void rlc_parsed_operator_expression_destroy(
	struct RlcParsedOperatorExpression * this);

/** Parses an operator expression.
@memberof RlcParsedOperatorExpression
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	`Null`, if no expression could be parsed, otherwise a pointer to a dynamically allocated expression, which is either an operator expression, or a more primitive expression in case no operator operator was present. */
_Nodiscard struct RlcParsedExpression * rlc_parsed_operator_expression_parse(
	struct RlcParser * parser);

/** Adds an expression to an operator expression's list.
@memberof RlcParsedOperatorExpression
@param[in,out] this:
	The operator expression to add an expression to.
	@dassert @nonnull
@param[in] expression:
	The expression to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_operator_expression_add(
	struct RlcParsedOperatorExpression * this,
	struct RlcParsedExpression * expression);

void rlc_parsed_operator_expression_print(
	struct RlcParsedOperatorExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif