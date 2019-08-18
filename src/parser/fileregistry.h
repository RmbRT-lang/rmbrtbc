#ifndef __rlc_parser_fileregistry_h_defined
#define __rlc_parser_fileregistry_h_defined

#include "file.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/** A registry entry for files that could not be parsed. */
struct RlcFailedParsedFile
{
	/** The file's name. */
	char const * fName;
	/** Whether the file failed with an error, or just didn't exist. */
	int fError;
};

/** A registry for parsed files. */
struct RlcParsedFileRegistry
{
	/** File names that failed with and error. */
	struct RlcFailedParsedFile * fFailedFiles;
	/** How many files failed with an error. */
	size_t fFailedFileCount;
	/** The parsed files. */
	struct RlcParsedFile ** fFiles;
	/** How many files were parsed. */
	size_t fFileCount;
};

/** Creates a parsed file registry.
@memberof RlcParsedFileRegistry
@param[out] this:
	The parsed file registry to create.
	@dassert @nonnull */
void rlc_parsed_file_registry_create(
	struct RlcParsedFileRegistry * this);

/** Destroys a parsed file registry.
@memberof RlcParsedFileRegistry
@param[in,out] this:
	The parsed file registry to destroy.
	@dassert @nonnull */
void rlc_parsed_file_registry_destroy(
	struct RlcParsedFileRegistry * this);

/** Retrieves a parsed file from the file registry.
	If the requested file did not exist, tries to parse it.
@memberof RlcParsedFileRegistry
@param[in,out] this:
	The file registry to retrieve a file from.
	@dassert @nonnull
@param[in] file:
	The file to look up.
	@dassert @nonnull
@param[out] error:
	Whether the file was found, but an error occurred.
@return
	The requested file, or, if it does not exist or contained errors, null. */
struct RlcParsedFile const * rlc_parsed_file_registry_get(
	struct RlcParsedFileRegistry * this,
	char const * file,
	int * error);

#ifdef __cplusplus
}
#endif

#endif