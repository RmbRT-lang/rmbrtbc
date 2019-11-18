#include "fileregistry.h"
#include "../assert.h"
#include "../malloc.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/** Retrieves the absolute path of a file path.
@return
	NULL if the requested file does not exist. */
static char const * to_absolute_path(char const * path)
{
	char buffer[PATH_MAX+1];
	char const * abs_path = realpath(path, buffer);
	if(!abs_path)
		return NULL;

	int abs_path_len = strlen(abs_path);
	char * ret = NULL;
	rlc_malloc((void**)&ret, abs_path_len + 1);
	memcpy(ret, abs_path, abs_path_len + 1);

	return ret;
}

/** Finds the parent directory name of a file or directory.
@return
	0 on failure, otherwise, the index of the '/' after the parent directory. */
static size_t parent_dir(char const * file)
{
	RLC_DASSERT(file != NULL);

	int len = strlen(file);
	while(len && file[len] != '/')
		--len;
	return len;
}

static char const * concat_paths(
	char const * base_file,
	size_t base_file_length,
	char const * relative_path)
{
	RLC_DASSERT(base_file != NULL);
	RLC_DASSERT(relative_path != NULL);

	if(!base_file_length)
		base_file_length = strlen(base_file);

	int need_slash = (base_file[base_file_length] != '/');
	size_t relative_path_length = strlen(relative_path);

	char * concat = NULL;
	rlc_malloc((void**)&concat, base_file_length + need_slash + relative_path_length + 1);

	memcpy(concat, base_file, base_file_length);
	if(need_slash)
		concat[base_file_length] = '/';
	memcpy(
		concat + base_file_length + need_slash,
		relative_path,
		relative_path_length);

	concat[base_file_length + need_slash + relative_path_length] = '\0';

	return concat;
}

/** Resolves a relative file path with respect to a base file path.
@param[in] base_file:
	The base file path.
@param[in] base_file_length:
	The string length of base_file to use. 0 if it should be detected automatically. */
static char const * resolve_relative_path(
	char const * base_file,
	size_t base_file_length,
	char const * relative_path)
{

	char const * concat = concat_paths(base_file, base_file_length, relative_path);
	char const * ret = to_absolute_path(concat);
	rlc_free((void**)&concat);
	return ret;
}

void rlc_resolved_file_registry_create(
	struct RlcResolvedFileRegistry * this)
{
	RLC_DASSERT(this != NULL);

	this->fBaseDir = to_absolute_path(".");
	RLC_ASSERT(this->fBaseDir && "failed to retrieve working directory");
	this->fFiles = NULL;
	this->fFileCount = 0;
}

void rlc_resolved_file_registry_destroy(
	struct RlcResolvedFileRegistry * this);

/** Queries a file from the registry.
	If the file was not registered, tries to parse and resolve it. */
struct RlcResolvedFile const * rlc_resolved_file_registry_get(
	struct RlcResolvedFileRegistry * this,
	char const * file)
{
	for(size_t i = 0; i < this->fFileCount; i++)
	{
		if(!strcmp(this->fFiles[i]->path, file))
			return this->fFiles[i];
	}

	struct RlcParsedFile * parsed = rlc_parsed_file_registry_get(
		&this->fParseRegistry,
		file);

	if(!parsed)
		return NULL;

	rlc_realloc(
		(void**)&this->fFiles,
		sizeof(struct RlcResolvedFile *) * ++this->fFileCount);
	struct RlcResolvedFile * resolved = NULL;
	rlc_malloc((void**)&resolved, sizeof(struct RlcResolvedFile));
	this->fFiles[this->fFileCount-1] = resolved;

	RLC_ASSERT(!"Now resolve the file");

	return resolved;
}