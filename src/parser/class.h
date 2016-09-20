/** @file class.h
	Contains the definition of the class type used by the parser. */
#ifndef __rlc_parser_class_h_defined
#define __rlc_parser_class_h_defined

#include "scopeentry.h"
#include "templatedecl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The class type as used by the parser.
@extends RlcParsedScopeEntry */
struct RlcParsedClass
{
	RLC_DERIVE(struct,RlcParsedScopeEntry);
	/** The template declarations of the class. */
	struct RlcTemplateDecl fTemplateDecl;
};

/** Destroys a parsed class.
@memberof RlcParsedClass
@param[in] this:
	The parsed class to destroy. */
void rlc_parsed_class_destroy(
	struct RlcParsedClass * this);

#ifdef __cplusplus
}
#endif

#endif