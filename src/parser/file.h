/** @file file.h
	Contains the parsed input file type. */
#ifndef __rlc_parser_file_h_defined
#define __rlc_parser_file_h_defined

#include "scopeentry.h"
#include "includestatement.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A parsed input file. */
struct RlcParsedFile
{
	/** The file's include statements. */
	struct RlcParsedIncludeStatement * fIncludes;
	/** The file's include statement count.*/
	size_t fIncludeCount;
	/** The file's parsed scope entries. */
	struct RlcParsedScopeEntryList fScopeEntries;

	/** The file's parser and tokens. */
	struct RlcParserData fParser;
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
	Whether there were any parsing errors. */
int rlc_parsed_file_create(
	struct RlcParsedFile * this,
	struct RlcPreprocessedFile * file);

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

#ifdef __cplusplus
}
#endif

#endif