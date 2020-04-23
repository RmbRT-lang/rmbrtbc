/** @file value.h
	Contains the value type that encompasses all primitive values. */
#ifndef __rlc_resolver_value_h_defined
#define __rlc_resolver_value_h_defined

#include "../scoper/number.h"
#include "../scoper/string.h"
#include "../scoper/expression.h"
#include "resolver.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RlcScopedScope;


/** Represents a primitive value's type. */
enum RlcValueType
{
	kRlcValueTypeBoolean,
	kRlcValueTypeNumber,
	kRlcValueTypeString,
	kRlcValueTypeCharacter,
	RLC_ENUM_END(RlcValueType)
};

/** Represents a primitive value. */
struct RlcValue
{
	/** The value's type. */
	enum RlcValueType type;

	union
	{
		int boolean;
		struct RlcNumber number;
		struct RlcScopedText string;
		struct RlcScopedText character;
	};
};

/** Compares two values.
	Both values must be of the same type. If they are not, then terminates with an error message.
@memberof RlcValue */
int rlc_value_cmp(
	struct RlcValue const * lhs,
	struct RlcValue const * rhs,
	struct RlcResolverFailContext const * context);

/** Compares two values.
	Both values may be of different types.
@memberof RlcValue*/
int rlc_value_order(
	struct RlcValue const * lhs,
	struct RlcValue const * rhs);

void rlc_value_destroy(
	struct RlcValue * this);

/** Checks whether a scoped expression holds a compile-time constant value.
@memberof RlcScopedExpression
@param[in] this:
	The scoped expression.
	@dassert @nonnull
@param[in] scope:
	The scope in which to look up the expression's symbols.
	@dassert @nonnull*/
int rlc_scoped_expression_is_constant(
	struct RlcScopedExpression const * this,
	struct RlcScopedScope const * scope);

#ifdef __cplusplus
}
#endif

#endif