#ifndef __rlc_resolver_string_h_defined
#define __rlc_resolver_string_h_defined

#include "../unicode.h"

struct RlcResolvedString
{
	/** The string's raw data (with byte order). */
	char const * fRawData;
	/** The string's character content. */
	rlc_char_t const * fRlcContent;
	/** The string's character content length. */
	size_t fLength;
};

#endif