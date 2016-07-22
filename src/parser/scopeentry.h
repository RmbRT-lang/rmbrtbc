#ifndef __rlc_parser_scopeentry_h_defined
#define __rlc_parser_scopeentry_h_defined


#include <stddef.h>

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Determines the deriving type of a RlcParsedScopeEntry. */
enum RlcParsedScopeEntryType
{
	/** RlcParsedClass */
	kRlcParsedClass,
	/** RlcParsedStruct */
	kRlcParsedStruct,
	/** RlcParsedRawtype */
	kRlcParsedRawtype,
	/** RlcParsedUnion */
	kRlcParsedUnion,
	/** RlcParsedNamespace */
	kRlcParsedNamespace,
	/** RlcParsedFunction */
	kRlcParsedFunction,
	/** RlcParsedVariable */
	kRlcParsedVariable,
	/** RlcParsedEnum */
	kRlcParsedEnum,
	/** RlcParsedTypedef */
	kRlcParsedTypedef,

	RLC_ENUM_END(RlcParsedScopeEntryType)
};

/** The base type of scope entries used in the parser.
	Contains a name and a declaration position in the source file.
@interface RlcParsedScopeEntry */
struct RlcParsedScopeEntry
{
	/** Determines the deriving type. */
	enum RlcParsedScopeEntryType const fRlcParsedScopeEntry;
	/** Where the scope entry was first declared in the source file. */
	size_t fDeclarationIndex;
	/** The name token index. */
	size_t fNameToken;
};

/** Destroys a parsed scope entry.
@pure @memberof RlcParsedScopeEntry

@param[in] this:
	The parsed scope entry to destroy. */
void rlc_parsed_scope_entry_destroy(
	struct RlcParsedScopeEntry * this);


#ifdef __cplusplus
}
#endif

#endif