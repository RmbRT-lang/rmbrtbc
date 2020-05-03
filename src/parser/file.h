/** @file file.h
	Contains the parsed input file type. */
#ifndef __rlc_parser_file_h_defined
#define __rlc_parser_file_h_defined

#include "scopeentry.h"
#include "includestatement.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RlcPrinter;

/** A parsed input file. */
struct RlcParsedFile
{
	/** The source file. */
	struct RlcSrcFile fSource;
	/** The file's include statements. */
	struct RlcParsedIncludeStatement * fIncludes;
	/** The file's include statement count.*/
	size_t fIncludeCount;
	/** The file's parsed scope entries. */
	struct RlcParsedScopeEntryList fScopeEntries;
};

/** Creates a parsed file from a preprocessed file.
@memberof RlcParsedFile
@param[out] this:
	The parsed file to create.
	@dassert @nonnull
@param[in] file:
	The preprocessed file to parse.
	@dassert @nonnull
	@pass_pointer_ownership
@return
	Whether the file exists. If there are any parsing errors, terminates the program. */
_Nodiscard int rlc_parsed_file_create(
	struct RlcParsedFile * this,
	char const * filename);

/** Destroys a parsed file.
@memberof RlcParsedFile
@param[in,out] this:
	The parsed file to destroy.
	@dassert @nonnull */
void rlc_parsed_file_destroy(
	struct RlcParsedFile * this);

/** Retrieves a parsed file's name.
@memberof RlcParsedFile
@param[in] this:
	The parsed file whose name to retrieve.
	@dassert @nonnull
@return
	The file's name. */
char const * rlc_parsed_file_name(
	struct RlcParsedFile const * this);

void rlc_parsed_file_print(
	struct RlcParsedFile * this,
	struct RlcPrinter const * printer);

#ifdef __cplusplus
}
#endif

#endif
