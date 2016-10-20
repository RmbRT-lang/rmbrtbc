/** @file struct.h
	Contains the struct type used by the parser. */
#ifndef __rlc_parser_struct_h_defined
#define __rlc_parser_struct_h_defined

#include "scopeentry.h"
#include "member.h"
#include "function.h"
#include "variable.h"

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/** Structure type as used by the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedStruct
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);

	/** The member functions. */
	struct RlcParsedMemberFunction * fFunctions;
	/** The member function count. */
	size_t fFunctionCount;
	/** The member variables. */
	struct RlcParsedMemberVariable * fVariables;
	/** The member variable count. */
	size_t fVariableCount;
};

/** Destroys a parsed struct.
@param[in] this:
	The struct to destroy. */
void rlc_parsed_struct_destroy(
	struct RlcParsedStruct * this);


#ifdef __cplusplus
}
#endif

#endif