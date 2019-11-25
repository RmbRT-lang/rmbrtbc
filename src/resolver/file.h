#ifndef __rlc_resolver_file_h_defined
#define __rlc_resolver_file_h_defined

#include "includestatement.h"
#include "scope.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Resolved source file. */
struct RlcResolvedFile
{
	char const * path;

	/** The source file's includes. */
	struct RlcIncludePathList includes;
	/** The source file's global namespace entries. */
	struct RlcResolvedScope * globalScope;
};

#ifdef __cplusplus
}
#endif

#endif