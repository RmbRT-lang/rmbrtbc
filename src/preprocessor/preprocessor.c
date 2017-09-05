#include "preprocessor.h"

#include "../assert.h"
#include "../malloc.h"

int rlc_preprocess(
	struct RlcFile const * source,
	struct RlcPreprocessorSymbols const * input,
	struct RlcPreprocessedFile * out)
{
	RLC_ASSERT(!"not implemented.");
}

void rlc_preprocessed_file_create_from_rlc_file(
	struct RlcPreprocessedFile * this,
	struct RlcFile const * source)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(source != NULL);

	this->fBaseFile = source;

	this->fTokens = NULL;
	this->fTokenCount = source->fTokenCount;

	if(this->fTokenCount)
		rlc_malloc(
			(void**)&this->fTokens,
			this->fTokenCount * sizeof(struct RlcToken *));

	for(size_t i = 0; i < this->fTokenCount; i++)
		this->fTokens[i] = &source->fTokens[i];
}

void rlc_preprocessed_file_destroy(
	struct RlcPreprocessedFile * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fTokens)
	{
		rlc_free((void**)&this->fTokens);
	}
	this->fTokenCount = 0;
	this->fBaseFile = NULL;
}