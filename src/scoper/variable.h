/** @file variable.h
	Contains the scoped variable type. */
#ifndef __rlc_scoper_variable_h_defined
#define __rlc_scoper_variable_h_defined

#include "scopeentry.h"
#include "member.h"
#include "../parser/variable.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcParsedVariable;

/** Scoped variable type. */
struct RlcScopedVariable {
	/** Whether the scoped variable has an explicit type. */
	char hasType;
	/** The number of initialiser arguments. */
	RlcSrcSize initArgCount;
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

/** Scoped global variable type.
@extends RlcScopedScopeEntry
@extends RlcScopedVariable */
struct RlcScopedGlobalVariable
{
	RLC_DERIVE(struct,RlcScopedScopeEntry);
	RLC_DERIVE(struct,RlcScopedVariable);
};

/** Scoped member variable type.
@extends RlcScopedMember
@extends RlcScopedVariable */
struct RlcScopedMemberVariable
{
	RLC_DERIVE(struct,RlcScopedMember);
	RLC_DERIVE(struct,RlcScopedVariable);
};

/** Creates a scoped global variable from a parsed global variable.
@memberof RlcScopedGlobalVariable
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
void rlc_scoped_global_variable_create(
	struct RlcScopedGlobalVariable * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariable const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped global variable.
@memberof RlcScopedGlobalVariable
@param[in,out] this:
	The scoped variable to destroy.
	@dassert @nonnull */
void rlc_scoped_global_variable_destroy(
	struct RlcScopedGlobalVariable * this);

/** Creates a scoped member variable from a parsed member variable.
@memberof RlcScopedMemberVariable */
void rlc_scoped_member_variable_create(
	struct RlcScopedMemberVariable * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMemberVariable const * parsed,
	struct RlcScopedScopeItemGroup * parent);

/** Destroys a scoped member variable.
@memberof RlcScopedMemberVariable
@param[in,out] this:
	The scoped member variable to destroy.
	@dassert @nonnulls */
void rlc_scoped_member_variable_destroy(
	struct RlcScopedMemberVariable * this);

#ifdef __cplusplus
}
#endif


#endif