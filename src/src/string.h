#ifndef __rlc_src_string_h_defined
#define __rlc_src_string_h_defined
#pragma once

#include <inttypes.h>
#include <stdio.h>

#include "../assert.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcFile;

/** Source character index type. */
typedef uint16_t RlcSrcIndex;
typedef uint16_t RlcSrcSize;

/** A string inside a source file. */
struct RlcSrcString
{
	RlcSrcIndex start;
	RlcSrcSize length;
} const kRlcSrcStringEmpty;

/** Checks whether a source string is a valid range inside a file.
@memberof RlcSrcString */
int rlc_src_string_valid(
	struct RlcSrcString const * this,
	struct RlcSrcFile const * file);

/** Compares two source strings within a source file.
@memberof RlcSrcString
@return
	`<0`, `0`, or `>0`, depending on the relation of `a` and `b`. */
int rlc_src_string_cmp(
	struct RlcSrcFile const * file,
	struct RlcSrcString const * a,
	struct RlcSrcString const * b);

/** Compares a source string within a source file with a C string.
@memberof RlcSrcString */
int rlc_src_string_cmp_cstr(
	struct RlcSrcFile const * files,
	struct RlcSrcString const * a,
	char const * cstr);

/** Copies a source string's contents into a dynamically allocated C string.
@memberof RlcSrcString
@return
	A dynamically allocated string holding the source string's contents. */
char const * rlc_src_string_cstr(
	struct RlcSrcString const * this,
	struct RlcSrcFile const * file);

/** Calculates the end of a source string.
@param[in] this:
	The source string.
	@dassert @nonnull
@return
	The source string's end. */
static inline RlcSrcIndex rlc_src_string_end(
	struct RlcSrcString const * this);

/** Prints a source string. */
void rlc_src_string_print(
	struct RlcSrcString const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#include "string.inl"

#ifdef __cplusplus
}
#endif

#endif