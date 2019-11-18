#include "includestatement.h"
#include "resolver.h"
#include "../assert.h"
#include "../malloc.h"
#include <string.h>


void rlc_include_path_list_create(
	struct RlcIncludePathList * this)
{
	RLC_DASSERT(this != NULL);

	this->fPaths = NULL;
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
	struct RlcSrcFile const * src)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(in != NULL);
	RLC_DASSERT(src != NULL);

	rlc_resolve_text(&this->fPath, src, &in->fFileName);
	if(this->fPath.fSymbolSize != 1)
		rlc_resolver_fail(&in->fFileName, src, "include path must be UTF-8");

	this->fIsRelative = in->fIsRelative;
}