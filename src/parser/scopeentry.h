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
	RLC_ABSTRACT(RlcParsedScopeEntry);
	
	/** Where the scope entry was first declared in the source file. */
	size_t fDeclarationIndex;
	/** The name tokens indices. */
	size_t * fNames;
	/** The name count. */
	size_t fNameCount;
};

/** Destroys a parsed scope entry.
	This function should only be called by deriving types in their destructor function.
@memberof RlcParsedScopeEntry
@param[in,out] this:
	The parsed scope entry to destroy.
	@dassert @nonnull */
void rlc_parsed_scope_entry_destroy_base(
	struct RlcParsedScopeEntry * this);

/** Looks up the derived type and destroys it.
@memberof RlcParsedScopeEntry
@param[in,out] this:
	The parsed scope entry to destroy.
	@dassert @nonnull */
void rlc_parsed_scope_entry_destroy_virtual(
	struct RlcParsedScopeEntry * this);

/** Creates an empty scope entry.
@param[in,out] this:
	The parsed scope entry to create.
	@dassert @nonnull */
void rlc_parsed_scope_entry_create(
	struct RlcParsedScopeEntry * this,
	enum RlcParsedScopeEntryType derivingType);

/** Adds a name to a scope entry.
@param[in,out] this:
	The scope entry to add a name to.
@param[in] name:
	The name to add. */
void rlc_parsed_scope_entry_add_name(
	struct RlcParsedScopeEntry * this,
	size_t name);


/** List of scope entries. */
struct RlcParsedScopeEntryList
{
	/** The list. */
	struct RlcParsedScopeEntry ** fEntries;
	/** The count. */
	size_t fEntryCount;
};

/** Creates an empty scope entry list.
@memberof RlcParsedScopeEntryList
@param[out] this:
	The list to create.
	@dassert @nonnull */
void rlc_parsed_scope_entry_list_create(
	struct RlcParsedScopeEntryList * this);

/** Adds an entry to a scope entry list.
@memberof RlcParsedScopeEntryList
@param[in,out] list:
	The list to add an entry to.
	@dassert @nonnull
@param[in] entry:
	The entry to add. @pass_pointer_ownership */
void rlc_parsed_scope_entry_list_add(
	struct RlcParsedScopeEntryList * list,
	struct RlcParsedScopeEntry * entry);

/** Destroys a scope entry list.
@memberof RlcParsedScopeEntryList
@param[in,out] this:
	The scope entry list to destroy.
	@dassert @nonnull */
void rlc_parsed_scope_entry_list_destroy(
	struct RlcParsedScopeEntryList * this);

#ifdef __cplusplus
}
#endif

#endif