#ifndef __rlc_resolver_string_h_defined
#define __rlc_resolver_string_h_defined

#include "../unicode.h"

struct RlcResolvedString
{
	/** The string's raw data (with correct byte order). */
	char const * fBytes;
	/** The byte size of the string. */
	size_t fByteCount;
};

#endif