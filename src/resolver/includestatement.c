#include "includestatement.h"
#include "resolver.h"
#include "fileregistry.h"
#include "../assert.h"
#include "../malloc.h"
#include "../fs.h"
#include <string.h>

void rlc_resolved_include_statement_destroy(
	struct RlcResolvedIncludeStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_resolved_text_destroy(&this->fPath);
}

void rlc_include_path_list_create(
	struct RlcIncludePathList * this)
{
	RLC_DASSERT(this != NULL);

	this->fPaths = NULL;
	this->fPathCount = 0;
}

void rlc_include_path_list_destroy(
	struct RlcIncludePathList * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fPaths)
		rlc_free((void**)&this->fPaths);
	this->fPathCount = 0;
}

void rlc_include_path_list_add(
	struct RlcIncludePathList * this,
	char const * path,
	int needs_free)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(path != NULL);

	// Check that the path does not yet exist.
	for(size_t i = 0; i < this->fPathCount; i++)
		if(!strcmp(path, this->fPaths[i].fPath))
			return;

	// Add the path.
	rlc_realloc(
		(void**)&this->fPaths,
		sizeof(struct RlcIncludePath) * ++this->fPathCount);

	this->fPaths[this->fPathCount-1].fNeedsFree = needs_free;
	this->fPaths[this->fPathCount-1].fPath = path;
}

void rlc_resolve_include_statement(
	struct RlcResolvedIncludeStatement * this,
	struct RlcParsedIncludeStatement const * in,
	struct RlcSrcFile const * src,
	struct RlcResolvedFileRegistry const * registry)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(in != NULL);
	RLC_DASSERT(src != NULL);
	RLC_DASSERT(registry != NULL);

	rlc_resolve_text(&this->fPath, src, &in->fFileName);
	if(this->fPath.fSymbolSize != 1)
		rlc_resolver_fail(&in->fFileName, src, "include path must be UTF-8");

	this->fIsRelative = in->fIsRelative;

	char const * inc_path = NULL;
	if(this->fIsRelative)
	{
		size_t parent_len = parent_dir(src->fName);
		inc_path = resolve_relative_path(
			src->fName, parent_len,
			this->fPath.fRaw, this->fPath.fElements);
	} else
	{
		for(RlcSrcSize i = 0; i < registry->fIncludeDirCount; i++)
		{
			inc_path = resolve_relative_path(
				registry->fIncludeDirs[i], 0,
				this->fPath.fRaw, this->fPath.fElements);

			if(inc_path)
				break;
		}
	}

	if(!inc_path)
	{
		rlc_resolver_fail(&in->fFileName, src, "failed to find %s include '%.*s'",
			this->fIsRelative ? "relative" : "global",
			(int)this->fPath.fElements,
			this->fPath.fRaw);
	}

	printf("%s: includes %s\n", src->fName, inc_path);
	rlc_free((void**)&inc_path);
}