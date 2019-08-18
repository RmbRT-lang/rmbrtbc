#include "fileregistry.h"
#include "../assert.h"
#include "../malloc.h"
#include "../tokeniser/tokens.h"
#include "../tokeniser/tokeniser.h"
#include "../file.h"
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
	char const * name,
	int error)
{
	rlc_realloc(
		(void**)&this->fFailedFiles,
		sizeof(struct RlcFailedParsedFile *) * ++this->fFailedFileCount);

	this->fFailedFiles[this->fFailedFileCount-1].fName = name;
	this->fFailedFiles[this->fFailedFileCount-1].fError = error;
}

struct RlcParsedFile const * rlc_parsed_file_registry_get(
	struct RlcParsedFileRegistry * this,
	char const * file,
	int * error)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	*error = 0;

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
			this->fFailedFiles[i].fName,
			file))
		{
			*error = this->fFailedFiles[i].fError;
			return NULL;
		}
	}

	// Try to parse the file.

	// Read the file.
	struct RlcFile * tokenised_file = NULL;
	rlc_malloc((void**)&tokenised_file, sizeof(struct RlcFile));
	{
		enum RlcFileResult result = rlc_read_text_file(file, &tokenised_file->fContents);
		if(result != kRlcFileSuccess)
		{
			*error = result != kRlcFileFailOpen;
			rlc_parsed_file_registry_add_failure(
				this,
				file,
				*error);
			rlc_free((void**)&tokenised_file);
			return NULL;
		}
	}

	tokenised_file->fContentLength = rlc_strlen(tokenised_file->fContents);
	tokenised_file->fName = file;

	// Tokenise the file.
	size_t error_index;
	{
		enum RlcTokResult result = rlc_tokenise(
			tokenised_file->fContents,
			tokenised_file,
			1, // Skip whitespaces.
			1, // Skip comments.
			&error_index);

		if(result != kRlcTokResultOk)
		{
			*error = 1;
			rlc_parsed_file_registry_add_failure(
				this,
				file,
				*error);
			return NULL;
		}
	}

	// Parse the file.
	struct RlcParsedFile * parsed_file = NULL;
	rlc_malloc((void**)&parsed_file, sizeof(struct RlcParsedFile));
	{
		struct RlcPreprocessedFile * preprocessed_file = NULL;
		rlc_malloc((void**)&preprocessed_file, sizeof(*preprocessed_file));
		rlc_preprocessed_file_create_from_rlc_file(
			preprocessed_file,
			tokenised_file);

		if(!rlc_parsed_file_create(parsed_file, preprocessed_file))
		{
			rlc_free((void**)&parsed_file);
			*error = 1;
			rlc_parsed_file_registry_add_failure(
				this,
				file,
				*error);
			return NULL;
		}
	}

	// Add the file to the registry and return it.
	rlc_parsed_file_registry_add_parsed_file(this, parsed_file);
	return parsed_file;
}