#include "fileregistry.h"
#include "../fs.h"
#include "../assert.h"
#include "../malloc.h"

#include <string.h>
#include <stdlib.h>

static void read_include_dirs(
	struct RlcScopedFileRegistry * this)
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

void rlc_scoped_file_registry_create(
	struct RlcScopedFileRegistry * this)
{
	RLC_DASSERT(this != NULL);

	this->fFiles = NULL;
	this->fFileCount = 0;
	this->fIncludeDirs = NULL;
	this->fIncludeDirCount = 0;
	read_include_dirs(this);
	rlc_parsed_file_registry_create(&this->fParseRegistry);
}

void rlc_scoped_file_registry_destroy(
	struct RlcScopedFileRegistry * this)
{
	rlc_parsed_file_registry_destroy(&this->fParseRegistry);
	for(RlcSrcIndex i = 0; i < this->fIncludeDirCount; i++)
		rlc_free((void**)&this->fIncludeDirs[i]);

	if(this->fIncludeDirs)
		rlc_free((void**)&this->fIncludeDirs);
	this->fIncludeDirCount = 0;

	for(RlcSrcIndex i = 0; i < this->fFileCount; i++)
	{
		rlc_scoped_file_destroy(this->fFiles[i]);
		rlc_free((void**)&this->fFiles[i]);
	}

	if(this->fFiles)
		rlc_free((void**)&this->fFiles);
	this->fFileCount = 0;
}

/** Queries a file from the registry.
	If the file was not registered, tries to parse and scope it. */
struct RlcScopedFile const * rlc_scoped_file_registry_get(
	struct RlcScopedFileRegistry * this,
	char const * file)
{
	for(size_t i = 0; i < this->fFileCount; i++)
	{
		if(!strcmp(this->fFiles[i]->path, file))
		{
			rlc_free((void*)&file);
			return this->fFiles[i];
		}
	}

	struct RlcParsedFile const * parsed = rlc_parsed_file_registry_get(
		&this->fParseRegistry,
		file);

	if(!parsed)
		return NULL;

	rlc_realloc(
		(void**)&this->fFiles,
		sizeof(struct RlcScopedFile *) * ++this->fFileCount);
	struct RlcScopedFile * scoped = NULL;
	rlc_malloc((void**)&scoped, sizeof(struct RlcScopedFile));
	this->fFiles[this->fFileCount-1] = scoped;

	scoped->globalScope = rlc_scoped_scope_new(NULL);
	scoped->fScoped = 0;
	scoped->path = file;
	rlc_include_path_list_create(&scoped->includes);
	{
		struct RlcScopedIncludeStatement inc_stmt;
		for(size_t i = 0; i < parsed->fIncludeCount; i++)
		{
			struct RlcScopedFile const * inc_file = rlc_scoped_file_registry_get(
				this,
				rlc_scope_include_statement(
					&inc_stmt,
					&parsed->fIncludes[i],
					&parsed->fSource,
					this));

			RLC_DASSERT(inc_file);

			rlc_scoped_include_statement_destroy(&inc_stmt);
		}
	}

	return scoped;
}