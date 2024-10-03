#ifndef __rlc_scoper_fileregistry_h_defined
#define __rlc_scoper_fileregistry_h_defined

#include <stddef.h>

#include "../parser/fileregistry.h"
#include "file.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedFileRegistry
{
	struct RlcScopedFile * * fFiles;
	size_t fFileCount;

	/** Include directories. */
	char const ** fIncludeDirs;
	/** Include directory count. */
	RlcSrcSize fIncludeDirCount;

	struct RlcParsedFileRegistry fParseRegistry;

	size_t fLineCount;
	size_t fBytes;
};

void rlc_scoped_file_registry_create(
	struct RlcScopedFileRegistry * this);

void rlc_scoped_file_registry_destroy(
	struct RlcScopedFileRegistry * this);

/** Queries a file from the registry.
	If the file was not registered, tries to parse and scope it. */
struct RlcScopedFile * rlc_scoped_file_registry_get(
	struct RlcScopedFileRegistry * this,
	char const * file);

char const * rlc_scoped_file_registry_resolve_global(
	struct RlcScopedFileRegistry const * this,
	char const * path,
	size_t path_len);

#ifdef __cplusplus
}
#endif

#endif