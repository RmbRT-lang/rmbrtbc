#include "fileregistry.h"
#include "../fs.h"
#include "../assert.h"
#include "../malloc.h"

#include <string.h>
#include <stdlib.h>

static void read_include_dirs(
	struct RlcResolvedFileRegistry * this)
{
	char const * dirs = getenv("RLINCLUDE");
	if(!dirs)
		return;

	char const * start = dirs;
	do
	{
		dirs = strchr(start, ':');
		int len = dirs ? (dirs - start) : (int)strlen(start);
		if(!len)
		{
			start = dirs+1;
			continue;
		}
		rlc_realloc(
			(void**)&this->fIncludeDirs,
			sizeof(char const *) * ++this->fIncludeDirCount);
		char * dir = NULL;
		rlc_malloc((void**)&dir, len + 1);
		memcpy(dir, start, len);
		dir[len] = '\0';
		this->fIncludeDirs[this->fIncludeDirCount-1] = dir;

		start = dirs + 1;
	} while(dirs);
}

void rlc_resolved_file_registry_create(
	struct RlcResolvedFileRegistry * this)
{
	RLC_DASSERT(this != NULL);

	this->fFiles = NULL;
	this->fFileCount = 0;
	this->fIncludeDirs = NULL;
	this->fIncludeDirCount = 0;
	read_include_dirs(this);
	rlc_parsed_file_registry_create(&this->fParseRegistry);
}

void rlc_resolved_file_registry_destroy(
	struct RlcResolvedFileRegistry * this)
{
	rlc_parsed_file_registry_destroy(&this->fParseRegistry);
	for(RlcSrcIndex i = 0; i < this->fIncludeDirCount; i++)
		rlc_free((void**)&this->fIncludeDirs[i]);

	if(this->fIncludeDirs)
		rlc_free((void**)&this->fIncludeDirs);
	this->fIncludeDirCount = 0;

	for(RlcSrcIndex i = 0; i < this->fFileCount; i++)
	{
		rlc_resolved_file_destroy(this->fFiles[i]);
		rlc_free((void**)&this->fFiles[i]);
	}

	if(this->fFiles)
		rlc_free((void**)&this->fFiles);
	this->fFileCount = 0;
}

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

	struct RlcParsedFile const * parsed = rlc_parsed_file_registry_get(
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

	resolved->path = to_absolute_path(file);
	rlc_include_path_list_create(&resolved->includes);
	{
		struct RlcResolvedIncludeStatement inc_stmt;
		for(size_t i = 0; i < parsed->fIncludeCount; i++)
		{
			rlc_resolve_include_statement(
				&inc_stmt,
				&parsed->fIncludes[i],
				&parsed->fSource,
				this);

			rlc_resolved_include_statement_destroy(&inc_stmt);
		}
	}
	return resolved;
}