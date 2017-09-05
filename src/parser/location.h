#ifndef __rlc_parser_location_h_defined
#define __rlc_parser_location_h_defined

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Location of a parsed object. */
struct RlcParseLocation
{
	/** The first token of the object. */
	size_t fBegin;
	/** The last token of the object. */
	size_t fEnd;
};


#ifdef __cplusplus
}
#endif

#endif