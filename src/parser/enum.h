/** @file enum.h
	Contains the enum struct as used by the parser. */
#ifndef __rlc_parser_enum_h_defined
#define __rlc_parser_enum_h_defined

#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Enum type.
@implements RlcParsedScopeEntry */
struct RlcParsedEnum
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);
};

/** Destroys an enum.
@memberof RlcParsedEnum
@param[in,out] this:
	The enum to destroy. */
void rlc_parsed_enum_destroy(
	struct RlcParsedEnum * this);

#ifdef __cplusplus
}
#endif


#endif