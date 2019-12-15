#ifndef __rlc_scoper_file_h_defined
#define __rlc_scoper_file_h_defined

#include "includestatement.h"
#include "scope.h"
#include "../parser/file.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Scoped source file. */
struct RlcScopedFile
{
	char const * path;

	/** The source file's includes. */
	struct RlcScopedIncludeList includes;
	/** Files including this file. */
	struct RlcScopedIncludeList includedBy;
	/** The source file's global namespace entries. */
	struct RlcScopedScope * globalScope;
};

void rlc_scoped_file_create(
	struct RlcScopedFile * this,
	char const * path,
	struct RlcParsedFile const * parsed);

void rlc_scoped_file_destroy(
	struct RlcScopedFile * this);

void rlc_scoped_file_populate_includes(
	struct RlcScopedFile * this,
	struct RlcScopedFileRegistry * registry,
	struct RlcParsedFile const * parsed);

#ifdef __cplusplus
}
#endif

#endif