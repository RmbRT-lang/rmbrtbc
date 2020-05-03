#include "fileregistry.h"
#include "../assert.h"
#include "../malloc.h"
#include "../tokeniser/tokens.h"
#include "../tokeniser/tokeniser.h"

#include <string.h>

void rlc_parsed_file_registry_create(
	struct RlcParsedFileRegistry * this)
{
	RLC_DASSERT(this != NULL);
	this->fFailedFiles = NULL;
	this->fFailedFileCount = 0;
	this->fFiles = NULL;
	this->fFileCount = 0;
}

void rlc_parsed_file_registry_destroy(
	struct RlcParsedFileRegistry * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fFailedFiles)
	{
		rlc_free((void**)&this->fFailedFiles);
		this->fFailedFileCount = 0;
	}

	if(this->fFiles)
	{
		for(size_t i = 0; i < this->fFileCount; i++)
		{
			rlc_parsed_file_destroy(this->fFiles[i]);
			rlc_free((void**)&this->fFiles[i]);
		}
		rlc_free((void**)&this->fFiles);
		this->fFileCount = 0;
	}
}

static void rlc_parsed_file_registry_add_parsed_file(
	struct RlcParsedFileRegistry * this,
	struct RlcParsedFile * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	rlc_realloc(
		(void**)&this->fFiles,
		sizeof(struct RlcParsedFile *) * ++this->fFileCount);
	this->fFiles[this->fFileCount-1] = file;
}

static void rlc_parsed_file_registry_add_failure(
	struct RlcParsedFileRegistry * this,
	char const * name)
{
	rlc_realloc(
		(void**)&this->fFailedFiles,
		sizeof(char const *) * ++this->fFailedFileCount);

	this->fFailedFiles[this->fFailedFileCount-1] = name;
}

struct RlcParsedFile * rlc_parsed_file_registry_get(
	struct RlcParsedFileRegistry * this,
	char const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	// Look whether the file was already parsed.
	for(size_t i = 0; i < this->fFileCount; i++)
	{
		if(!strcmp(
			rlc_parsed_file_name(this->fFiles[i]),
			file))
		{
			return this->fFiles[i];
		}
	}

	// Look whether the file failed already before.
	for(size_t i = 0; i < this->fFailedFileCount; i++)
	{
		if(!strcmp(
			this->fFailedFiles[i],
			file))
		{
			return NULL;
		}
	}

	// Try to parse the file.

	// Read the file.
	struct RlcParsedFile * parsed_file = NULL;
	rlc_malloc((void**)&parsed_file, sizeof(struct RlcParsedFile));

	if(!rlc_parsed_file_create(parsed_file, file))
	{
		rlc_parsed_file_registry_add_failure(this, file);
		return NULL;
	}

	// Add the file to the registry and return it.
	rlc_parsed_file_registry_add_parsed_file(this, parsed_file);
	return parsed_file;
}