/** @file variable.h
	Contains the scoped variable type. */
#ifndef __rlc_scoper_variable_h_defined
#define __rlc_scoper_variable_h_defined

#include "scopeentry.h"
#include "../parser/variable.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedVariable;

/** Scoped variable type.
@extends RlcScopedScopeEntry */
struct RlcScopedVariable
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);

	/** The variable's type. */
	union {
		struct RlcScopedTypeName name;
		enum RlcTypeQualifier autoQualifier;
	} type;
	/** The variable's reference type. */
	enum RlcReferenceType referenceType;
	/** The variable's intitialisation arguments. */
	struct RlcScopedExpression ** initArgs;
};

/** Creates a scoped variable from a parsed variable.
@memberof RlcScopedVariable
@param[out] this:
	The scoped variable.
	@dassert @nonnull
@param[in] file:
	The parsed variable's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed variable.
	@dassert @nonnull
@param[in] parent:
	The variable's scope item group.
	@dassert @nonnull */
void rlc_scoped_variable_create(
	struct RlcScopedVariable * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariable const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped variable.
@memberof RlcScopedVariable
@param[in,out] this:
	The scoped variable to destroy.
	@dassert @nonnull */
void rlc_scoped_variable_destroy(
	struct RlcScopedVariable * this);

#ifdef __cplusplus
}
#endif


#endif