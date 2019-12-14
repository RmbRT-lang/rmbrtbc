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


/** An include path. */
struct RlcIncludePath
{
	/** Whether the path needs to be freed or not. */
	int fNeedsFree;
	/** The include path. */
	char const * fPath;
};


struct RlcIncludePathList
{
	struct RlcIncludePath * fPaths;
	size_t fPathCount;
};

/** Creates an include path list.
@memberof RlcIncludePathList
@param[out] this:
	The include path list to create.
	@dassert @nonnull */
void rlc_include_path_list_create(
	struct RlcIncludePathList * this);
/** Destroys an include path list.
@memberof RlcIncludePathList
@param[in,out] this:
	The include path list to destroy.
	@dassert @nonnull */
void rlc_include_path_list_destroy(
	struct RlcIncludePathList * this);

/** Adds an include path to an include path list.
@memberof RlcIncludePathList
@param[in,out] this:
	The include path list to add a path to.
	@dassert @nonnull
@param[in] path:
	The include path to add.
	@dassert @nonnull
@param[in] needs_free:
	Whether the include path must be freed later on. */
void rlc_include_path_list_add(
	struct RlcIncludePathList * this,
	char const * path,
	int needs_free);

struct RlcScopedFileRegistry;

void rlc_scope_include_statement(
	struct RlcScopedIncludeStatement * this,
	struct RlcParsedIncludeStatement const * in,
	struct RlcSrcFile const * src,
	struct RlcScopedFileRegistry const * registry);

#ifdef __cplusplus
}
#endif


#endif