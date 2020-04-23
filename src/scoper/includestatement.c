#include "includestatement.h"
#include "../resolver/resolver.h"
#include "fileregistry.h"
#include "../assert.h"
#include "../malloc.h"
#include "../fs.h"
#include <string.h>

void rlc_scoped_include_statement_destroy(
	struct RlcScopedIncludeStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_text_destroy(&this->fPath);
}

void rlc_scoped_include_list_create(
	struct RlcScopedIncludeList * this)
{
	RLC_DASSERT(this != NULL);

	this->fPaths = NULL;
	this->fPathCount = 0;
}

void rlc_scoped_include_list_destroy(
	struct RlcScopedIncludeList * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fPaths)
		rlc_free((void**)&this->fPaths);
	this->fPathCount = 0;
}

struct RlcScopedInclude * rlc_scoped_include_list_add(
	struct RlcScopedIncludeList * this,
	struct RlcScopedFile * file,
	int connected)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	for(RlcSrcIndex i = 0; i < this->fPathCount; i++)
		if(file == this->fPaths[i].fFile)
		{
			// If it is already connected, do not set to unconnected again.
			this->fPaths[i].fConnected |= connected;
			return &this->fPaths[i];
		}

	// Add the path.
	rlc_realloc(
		(void**)&this->fPaths,
		sizeof(struct RlcScopedInclude) * ++this->fPathCount);

	this->fPaths[this->fPathCount-1].fConnected = connected;
	this->fPaths[this->fPathCount-1].fFile = file;

	return &this->fPaths[this->fPathCount-1];
}

char const * rlc_scope_include_statement(
	struct RlcScopedIncludeStatement * this,
	struct RlcParsedIncludeStatement const * in,
	struct RlcSrcFile const * src,
	struct RlcScopedFileRegistry const * registry)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(in != NULL);
	RLC_DASSERT(src != NULL);
	RLC_DASSERT(registry != NULL);

	rlc_scoped_text_create(&this->fPath, src, &in->fFileName);
	if(this->fPath.fSymbolSize != 1)
		rlc_resolver_fail(&in->fFileName.content, src, "include path must be UTF-8");

	if(!this->fPath.fElements)
		rlc_resolver_fail(&in->fFileName.content, src, "invalid empty include path");
	if(((char const*)this->fPath.fRaw)[0] == '/')
		rlc_resolver_fail(&in->fFileName.content, src, "include paths must not start with /");

	this->fIsRelative = in->fIsRelative;

	char const * resolved_path = NULL;
	if(this->fIsRelative)
	{
		size_t parent_len = parent_dir(src->fName);
		resolved_path = resolve_relative_path(
			src->fName, parent_len,
			this->fPath.fRaw, this->fPath.fElements);
	} else
	{
		for(RlcSrcSize i = 0; i < registry->fIncludeDirCount; i++)
		{
			resolved_path = resolve_relative_path(
				registry->fIncludeDirs[i], 0,
				this->fPath.fRaw, this->fPath.fElements);

			if(resolved_path)
				break;
		}
	}

	if(!resolved_path)
	{
		rlc_resolver_fail(&in->fFileName.content, src, "failed to find %s include '%.*s'",
			this->fIsRelative ? "relative" : "global",
			(int)this->fPath.fElements,
			this->fPath.fRaw);
	}

	printf("%s: includes %s\n", src->fName, resolved_path);

	return resolved_path;
}