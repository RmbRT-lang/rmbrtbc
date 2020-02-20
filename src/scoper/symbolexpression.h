/** @file symbolexpression.h
	Contains the scoped symbol expression type. */
#ifndef __rlc_scoper_symbolexpression_h_defined
#define __rlc_scoper_symbolexpression_h_defined
#pragma once

#include "expression.h"
#include "symbol.h"

#ifdef __cplusplus
extern "C" {
#endif

#define kRlcScopedSymbolExpression kRlcParsedSymbolExpression

/** A scoped symbol expression.
@implements RlcScopedExpression */
struct RlcScopedSymbolExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);
	struct RlcScopedSymbol fName;
};

struct RlcParsedSymbolExpression;

/** Creates a new symbol expression from a parsed symbol expression.
@memberof RlcScopedExpression
@param[in] parsed:
	The parsed symbol expression.
	@dassert @nonnull
@param[in] file:
	The expression's source file.
	@dassert @nonnull
@return
	The new symbol expression. */
struct RlcScopedSymbolExpression * rlc_scoped_symbol_expression_new(
	struct RlcParsedSymbolExpression const * parsed,
	struct RlcSrcFile const * file);

/** Destroys and deletes a symbol expression.
@memberof RlcScopedSymbolExpression
@param[in,out] this:
	The symbol expression to delete.
	@dassert @nonnull */
void rlc_scoped_symbol_expression_delete(
	struct RlcScopedSymbolExpression * this);

#ifdef __cplusplus
}
#endif

#endif