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
@implements RlcParsedScopeEntry */
struct RlcParsedClass
{
	/** T*/
	struct RlcParsedScopeEntry fParsedScopeEntry;
	struct RlcTemplateDecl fTemplateDecl;
};

void rlc_parsed_class_destroy(
	struct RlcParsedClass * this);

#ifdef __cplusplus
}
#endif

#endif