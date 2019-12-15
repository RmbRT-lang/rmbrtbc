/** @file includestatement.h
	Contains scoped include statements. */
#ifndef __rlc_scoper_includestatement_h_defined
#define __rlc_scoper_includestatement_h_defined

#include "../macros.h"
#include "../parser/includestatement.h"
#include "string.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedIncludeStatement
{
	struct RlcScopedText fPath;
	int fIsRelative;
};

void rlc_scoped_include_statement_destroy(
	struct RlcScopedIncludeStatement * this);

struct RlcScopedFile;
/** An include path. */
struct RlcScopedInclude
{
	/** The included file. */
	struct RlcScopedFile * fFile;
	/** Whether the scopes have been connected. */
	int fConnected;
};

struct RlcScopedIncludeList
{
	struct RlcScopedInclude * fPaths;
	RlcSrcSize fPathCount;
};

/** Creates an include path list.
@memberof RlcIncludePathList
@param[out] this:
	The include path list to create.
	@dassert @nonnull */
void rlc_scoped_include_list_create(
	struct RlcScopedIncludeList * this);
/** Destroys an include path list.
@memberof RlcIncludePathList
@param[in,out] this:
	The include path list to destroy.
	@dassert @nonnull */
void rlc_scoped_include_list_destroy(
	struct RlcScopedIncludeList * this);

/** Adds an include path to an include path list.
	If it was already added, overwrites the entry.
@memberof RlcIncludePathList
@param[in,out] this:
	The include path list to add a path to.
	@dassert @nonnull
@param[in] path:
	The include path to add.
	@dassert @nonnull
@param[in] connected:
	Whether the included file has already been connected. */
struct RlcScopedInclude * rlc_scoped_include_list_add(
	struct RlcScopedIncludeList * this,
	struct RlcScopedFile * file,
	int connected);

struct RlcScopedFileRegistry;

char const * rlc_scope_include_statement(
	struct RlcScopedIncludeStatement * this,
	struct RlcParsedIncludeStatement const * in,
	struct RlcSrcFile const * src,
	struct RlcScopedFileRegistry const * registry);

#ifdef __cplusplus
}
#endif


#endif