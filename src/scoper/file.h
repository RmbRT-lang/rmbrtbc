#ifndef __rlc_scoper_file_h_defined
#define __rlc_scoper_file_h_defined

#include "includestatement.h"
#include "../parser/file.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcPrinter;
struct RlcScopedFileRegistry;

/** Scoped source file. */
struct RlcScopedFile
{
	char const * path;

	struct RlcParsedFile * parsed;

	/** The source file's includes. */
	struct RlcScopedIncludeList includes;
	/** Files including this file. */
	struct RlcScopedIncludeList includedBy;
	/** The last compilation unit this file has been printed in. */
	unsigned lastPrinted;
};

void rlc_scoped_file_create(
	struct RlcScopedFile * this,
	char const * path,
	struct RlcParsedFile * parsed);

void rlc_scoped_file_destroy(
	struct RlcScopedFile * this);

void rlc_scoped_file_populate_includes(
	struct RlcScopedFile * this,
	struct RlcScopedFileRegistry * registry,
	struct RlcParsedFile const * parsed);

void rlc_scoped_file_print(
	struct RlcScopedFile * this,
	struct RlcScopedFileRegistry * registry,
	struct RlcPrinter const * printer);

#ifdef __cplusplus
}
#endif

#endif