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
	Contains a name and a declaration position in the source file. */
struct RlcParsedScopeEntry
{
	/** Determines the deriving type. */
	enum RlcParsedScopeEntryType const fRlcParsedScopeEntry;
	/** Where the scope entry was first declared in the source file. */
	size_t fDeclarationIndex;
	/** The name tokens indices. */
	size_t * fNames;
	/** The name count. */
	size_t fNameCount;
};

/** Destroys a parsed scope entry.
@memberof RlcParsedScopeEntry
@param[in,out] this:
	The parsed scope entry to destroy.
	@dassert @nonnull */
void rlc_parsed_scope_entry_destroy(
	struct RlcParsedScopeEntry * this);

/** Creates an empty scope entry.
@param[in,out] this:
	The parsed scope entry to create.
	@dassert @nonnull */
void rlc_parsed_scope_entry_create(
	struct RlcParsedScopeEntry * this);

/** Adds a name to a scope entry.
@param[in,out] this:
	The scope entry to add a name to.
@param[in] name:
	The name to add. */
void rlc_parsed_scope_entry_add_name(
	struct RlcParsedScopeEntry * this,
	size_t name);


#ifdef __cplusplus
}
#endif

#endif