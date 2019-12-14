#ifndef __rlc_scoper_file_h_defined
#define __rlc_scoper_file_h_defined

#include "../scoper/includestatement.h"
#include "scope.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Scoped source file. */
struct RlcScopedFile
{
	char const * path;
	/** Whether the file is scoped. */
	int fScoped;

	/** The source file's includes. */
	struct RlcIncludePathList includes;
	/** The source file's global namespace entries. */
	struct RlcScopedScope * globalScope;
};

void rlc_scoped_file_create(
	struct RlcScopedFile * this);
void rlc_scoped_file_destroy(
	struct RlcScopedFile * this);

#ifdef __cplusplus
}
#endif

#endif