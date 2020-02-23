/** @file value.h
	Contains the value type that encompasses all primitive values. */
#ifndef __rlc_resolver_value_h_defined
#define __rlc_resolver_value_h_defined

#include "../scoper/number.h"
#include "../scoper/string.h"
#include "resolver.h"

#ifdef __cplusplus
extern "C" {
#endif

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
@memberof RlcValue */
int rlc_value_cmp(
	struct RlcValue const * lhs,
	struct RlcValue const * rhs,
	struct RlcResolverFailContext const * context);

#ifdef __cplusplus
}
#endif

#endif