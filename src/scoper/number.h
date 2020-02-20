/** @file number.h
	Contains the integer type used to represent numbers. */
#ifndef __rlc_scoper_number_h_defined
#define __rlc_scoper_number_h_defined

#include "../macros.h"
#include <inttypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RlcToken;
struct RlcSrcFile;
struct RlcResolverFailContext;

/** Internal number type.
	This type is used for compile-time calculations (such as constant expressions or numeric template arguments). This is implemented as a 64-bit signed integer. Any operations that result in an over/underflow will result in a compilation error. */
struct RlcNumber
{
	/** The integer's value. */
	int64_t value;
} const kRlcNumberZero;

/** Creates a number from a number token.
@memberof RlcNumber */
void rlc_number_from_token(
	struct RlcNumber * this,
	struct RlcSrcFile const * file,
	struct RlcToken const * token);

/** Adds two numbers.
@memberof RlcNumber */
void rlc_number_add(
	struct RlcNumber * dst,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context);

/** Subtracts `rhs` from `lhs`.
@memberof RlcNumber */
void rlc_number_sub(
	struct RlcNumber * dst,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context);

/** Multiplies two numbers.
@memberof RlcNumber */
void rlc_number_mul(
	struct RlcNumber * dst,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context);

/** Divides two numbers.
@memberof RlcNumber */
void rlc_number_div(
	struct RlcNumber * quo,
	struct RlcNumber * rem,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context);

/** Negates a number.
@memberof RlcNumber */
void rlc_number_neg(
	struct RlcNumber * dst,
	struct RlcNumber const * src,
	struct RlcResolverFailContext const * context);

/** Compares two numbers.
@memberof RlcNumber */
_Nodiscard int rlc_number_cmp(
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs);


/** Prints a number.
@memberof RlcNumber */
void rlc_number_fprint(
	struct RlcNumber const * this,
	FILE * file);

#ifdef __cplusplus
}
#endif

#endif