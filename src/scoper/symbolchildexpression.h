/** @file symbolchildexpression.h
	Contains the scoped symbol child expression type. */
#ifndef __rlc_scoper_symbolchildexpression_h_defined
#define __rlc_scoper_symbolchildexpression_h_defined
#pragma once

#include "expression.h"
#include "symbol.h"


#ifdef __cplusplus
extern "C" {
#endif


struct RlcSrcFile;
struct RlcParsedSymbolChildExpression;

#define kRlcScopedSymbolChildExpression kRlcParsedSymbolChildExpression

/** Scoped symbol child expression type.
@extends RlcScopedExpression */
struct RlcScopedSymbolChildExpression
{
	RLC_DERIVE(struct,RlcScopedExpression);
	/** The expression's symbol child. */
	struct RlcScopedSymbolChild symbol;
};

/** Creates a scoped symbol child expression from a parsed symbol child expression.
@memberof RlcScopedSymbolChildExpression
@param[in] file:
	The sc */
struct RlcScopedSymbolChildExpression * rlc_scoped_symbol_child_expression_new(
	struct RlcParsedSymbolChildExpression const * parsed,
	struct RlcSrcFile const * file);

/** Destroys and deletes a scoped symbol child expression.
@memberof RlcScopedSymbolChildExpression
@param[in,out] this:
	The scoped symbol child expression to destroy.
	@dassert @nonnull */
void rlc_scoped_symbol_child_expression_delete(
	struct RlcScopedSymbolChildExpression * this);

#ifdef __cplusplus
}
#endif

#endif