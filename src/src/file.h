#ifndef __rlc_src_file_h_defined
#define __rlc_src_file_h_defined
#pragma once

#include <stddef.h>
#include <stdio.h>

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A RL source file. */
struct RlcSrcFile
{
	char * fName;
	char * fContents;
	size_t fContentLength;
};

/** Reads a source file.
@memberof RlcSrcFile
@param[out] this:
	The source file to read into. Holds no valid value if the operation fails.
	@dassert @nonnull
@param[in] file:
	The file path to read from. This will also be the source file's name.
	@dassert @nonnull
@return
	Whether the file could be successfully read. */
int rlc_src_file_read(
	struct RlcSrcFile * this,
	char const * file);

/** Destroys a source file.
@param[in,out] this:
	The source file to destroy.
	@dassert @nonnull */
void rlc_src_file_destoy(
	struct RlcSrcFile * this);

/** Retrieves a line of code of a source file.
@param[in] this:
	The file whose line to read.
	@dassert @nonnull
@param[in] index:
	A source index that is within the line.
	@dassert must be within the file.
@return
	The line containing `index`. */
struct RlcSrcString rlc_src_file_line(
	struct RlcSrcFile const * this,
	RlcSrcIndex index);

/** A 1-based line:column position in a source file. */
struct RlcSrcPosition
{
	unsigned line;
	unsigned column;
};

/** Calculates the line and column of a character in a source file.
@memberof RlcSrcFile
@param[in] this:
	The source file.
	@dassert @nonnull
@param[out] out:
	The position to calculate.
	@dassert @nonnull
@param[in] index:
	The source index. */
void rlc_src_file_position(
	struct RlcSrcFile const * this,
	struct RlcSrcPosition * out,
	RlcSrcIndex index);

#ifdef __cplusplus
}
#endif

#endif